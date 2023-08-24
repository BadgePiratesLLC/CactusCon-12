const axios = require("axios");
const bwipjs = require("bwip-js");
const fs = require("fs").promises;
const { spawn } = require("child_process");
const { promisify } = require("util");
const qrcodeTerminal = require("qrcode-terminal");
const readlineSync = require("readline-sync");
const sharp = require("sharp");
const { exec } = require("child_process");

// Promisify the toBuffer method
const toBufferAsync = promisify(bwipjs.toBuffer);

const CONFERENCE_ID = "64cadc29e1c00285843b4166";
const BMS_API_URL = "http://localhost:4200";

async function getTokenFromFile() {
  const data = await fs.readFile("./.token", "utf-8");
  return data.split("\n")[0];
}

async function saveTokenToFile(token) {
  await fs.writeFile("./.token", token, "utf-8");
}

async function login(username, password) {
  const response = await axios.post(
    `${BMS_API_URL}/api/users/login`,
    {
      username,
      password,
    },
    {
      headers: {
        "Content-Type": "application/json",
      },
    }
  );
  return response.data.token;
}

async function registerBadge(token) {
  const response = await axios.post(
    `${BMS_API_URL}/api/badges`,
    {
      conferenceId: CONFERENCE_ID,
    },
    {
      headers: { Authorization: `Bearer ${token}` },
      "Content-Type": "application/json",
    }
  );
  return response.data;
}

async function updateEmbeddedTextFile(badgeId) {
  try {
    await fs.writeFile("./data/badge.txt", badgeId);
    console.log("Badge ID successfully written to badge.txt");
  } catch (error) {
    console.error(`Error writing to file: ${error}`);
  }
}

async function runPlatformio() {
  return new Promise((resolve, reject) => {
    const platformio = spawn("pio", ["run", "-t", "upload"], { cwd: "../" });

    platformio.stdout.on("data", (data) => {
      console.log(`stdout: ${data}`);
    });

    platformio.stderr.on("data", (data) => {
      console.error(`stderr: ${data}`);
    });

    platformio.on("close", (code) => {
      if (code !== 0) {
        return reject(new Error(`platformio process exited with code ${code}`));
      }
      resolve();
    });
  });
}

async function generateQRCode(badgeID) {
  // Construct the URL
  const url = `http://localhost:4201/badge/${badgeID}`;

  // Generate the QR code
  const pngBuffer = await toBufferAsync({
    bcid: "qrcode", // Barcode type
    text: url, // Text to encode
    scale: 1, // 3x scaling factor
    height: 5, // Bar height, in millimeters
    width: 5,
    includetext: false, // Show human-readable text
    textxalign: "center", // Always good to set this
  });

  return pngBuffer;
}

async function saveQRCodeToDisk(pngBuffer, badgeId) {
  const filename = `./qrcodes/${badgeId}.png`;
  try {
    await fs.writeFile(filename, pngBuffer);
    await sharp(filename)
      .extend({
        top: 0,
        bottom: 200,
        left: 50,
        right: 0,
        background: { r: 255, g: 255, b: 255, alpha: 1 },
      })
      .toFile(`./qrcodes/1-${badgeId}.png`, (err, info) => {
        if (err) console.error(err);
        else console.log(info);
      });
    return `/Users/patrickburns/Project-HomeBase/provisioning/qrcodes/1-${badgeId}.png`;
  } catch (err) {
    console.error("Error saving QR code:", err);
    return false;
  }
}

async function printImage(filePath) {
  exec(`lpr -P "QL_1100" ${filePath}`, (error, stdout, stderr) => {
    if (error) {
      console.error(`exec error: ${error}`);
      return;
    }
    if (stderr) {
      console.error(`stderr: ${stderr}`);
    }
    console.log(`stdout: ${stdout}`);
  });
}

async function main() {
  let token;

  try {
    token = await getTokenFromFile();
  } catch (error) {
    if (error.code === "ENOENT") {
      const username = readlineSync.question("Please enter your username: ");
      const password = readlineSync.question("Please enter your password: ", {
        hideEchoBack: true,
      });
      token = await login(username, password);
      await saveTokenToFile(token);
    } else {
      throw error;
    }
  }

  const badge = await registerBadge(token);
  console.log(`Registered badge with ID: ${badge._id}`);
  await updateEmbeddedTextFile(badge._id);
  await runPlatformio();
  const qrPNG = await generateQRCode(badge._id);
  const qrCodePath = await saveQRCodeToDisk(qrPNG, badge._id);
  qrcodeTerminal.generate(`${BMS_API_URL}/badge/${badge._id}`, { small: true });
  await printImage(qrCodePath);
}

main()
  .catch((error) => {
    console.error("An error occurred:", error);
    //TODO: Undo the badge registration here
  })
  .finally(() => {
    console.log("doneskies");
  });

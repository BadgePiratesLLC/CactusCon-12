# Use the official Node.js image
FROM amd64/node:14


# Set the working directory
WORKDIR /usr/src/app

# Copy package.json and package-lock.json (if exists)
COPY package*.json ./

# Install the dependencies
RUN npm install

# Copy the local code to the container
COPY . .

# Set the command to run your application
CMD [ "node", "index.js" ]

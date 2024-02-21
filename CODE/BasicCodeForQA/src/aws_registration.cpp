#include "aws_registration.hpp"

bool AWSRegistration::register_new_device(const char * device_id) {
    Preferences local_prefs;
    AWSDeviceCredentials creds;
    WiFiClientSecure *secure_client = new WiFiClientSecure;
    HTTPClient https;
    char url[80];
    int http_response_code;
    StaticJsonDocument<3000> doc;
    String response;

    sprintf(url, "https://%s/register?device_id=%s", AWS_REGISTRATION_ENDPOINT, device_id);
    printf("[AWSReg] Registering at %s" , url);

    secure_client -> setCACert(AWS_REGISTRATION_CA_CERT);
    secure_client -> setCertificate(AWS_REGISTRATION_CLIENT_CERT);
    secure_client -> setPrivateKey(AWS_REGISTRATION_PRIVATE_KEY);
    https.setReuse(true);
    https.useHTTP10(true);

    if (!https.begin(*secure_client, url)) {
        printf("[AWSReg] Failed HTTP connect\n");
        return false;
    }
    
    http_response_code = https.GET();
    
    if (!(http_response_code >= 200 && http_response_code < 300)) {
        printf("[AWSReg] Unexpected HTTP code %d %s\n", http_response_code, https.getString());
        return false;
    }

    deserializeJson(doc, https.getStream());
    if (!doc.as<String>()) {
        printf("[AWSReg] failed to parse JSON %s\n", https.getString());
        return false;
    }

    if (local_prefs.begin("aws", false)) {
        if (doc["PublicKey"].is<String>()) {
            local_prefs.putString("deviceCert", doc["PublicKey"].as<String>());
        }
        
        if (doc["PrivateKey"].is<String>()) {
            local_prefs.putString("deviceKey", doc["PrivateKey"].as<String>());
        }

        local_prefs.end();
        printf("[AWSReg] Successfully registered!\n");
    } else {
        printf("[AWSReg] Failed to open prefs file.\n");
    }
    secure_client -> stop();
    
    return true;
}

bool AWSRegistration::device_credentials_exist() {
    Preferences local_prefs;
    local_prefs.begin("aws", true);

    if (local_prefs.isKey("deviceCert") && local_prefs.isKey("deviceKey")) {
        local_prefs.end();
        return true;
    }

    local_prefs.end();
    return false;
}

void AWSRegistration::load_device_credentials(AWSDeviceCredentials &creds) {
    Preferences local_prefs;
    local_prefs.begin("aws", true);

    if (local_prefs.isKey("deviceCert")) {
        creds.device_cert = local_prefs.getString("deviceCert");
    }

    if (local_prefs.isKey("deviceKey")) {
        creds.device_key = local_prefs.getString("deviceKey");
    }

    local_prefs.end();
    printf("[AWSReg] Loaded client keys\n");
}

void AWSRegistration::clear_saved_credentials() {
    Preferences local_prefs;
    local_prefs.begin("aws", false);
    local_prefs.remove("deviceCert");
    local_prefs.remove("deviceKey");
    local_prefs.end();
}
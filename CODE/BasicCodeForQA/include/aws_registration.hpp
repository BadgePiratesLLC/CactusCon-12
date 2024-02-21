#pragma once
#include <Arduino.h>
#include <Preferences.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "aws_mqtt.hpp"

const char AWS_REGISTRATION_ENDPOINT[] = "badge.cactuscon.com";

const char AWS_REGISTRATION_CA_CERT[] = "-----BEGIN CERTIFICATE-----\n" \
"MIIDSzCCAjOgAwIBAgIUeVE11BE6qdkBOnMRaMgaAzhlLNUwDQYJKoZIhvcNAQEL\n" \
"BQAwFjEUMBIGA1UEAwwLQ0MxMUJhZGdlQ0EwHhcNMjMwOTI2MDA0NzU4WhcNMzMw\n" \
"OTIzMDA0NzU4WjAWMRQwEgYDVQQDDAtDQzExQmFkZ2VDQTCCASIwDQYJKoZIhvcN\n" \
"AQEBBQADggEPADCCAQoCggEBALL8da/q33VZ8d0CPkbTSJ9oj3NXTm7hkhmQyK9r\n" \
"YuITpWutcoH/FlQT0+GfpvaQc2UtM7V6CU/Os9fCL/5PfbrSPcMRYzIOWmpcKgVp\n" \
"rIUVu3KI2XRo/d733SXnEqUe4tsoe4WyvtcGhnJcZ4SO3bu8UzbCILGGJHWhLx/7\n" \
"jh42bacb9TNjLQjbtCkV1hPbfa2D7PQvyquxTMvf1wzGT/gsYAz9ur4WWaB5indy\n" \
"JVOYpgjjVbnSfXIWuW+6Zke3ypi/nrVNHaB9tgEiEJbcQK5OgEwiRtw6zd+Elp3b\n" \
"E/MC0I5RdHlC6j7BQwSfAEaNuUwVeuRCIZtvo7F0Zjzq1qMCAwEAAaOBkDCBjTAd\n" \
"BgNVHQ4EFgQU663mX42uVeID9bLej82EqQ3S1FowUQYDVR0jBEowSIAU663mX42u\n" \
"VeID9bLej82EqQ3S1FqhGqQYMBYxFDASBgNVBAMMC0NDMTFCYWRnZUNBghR5UTXU\n" \
"ETqp2QE6cxFoyBoDOGUs1TAMBgNVHRMEBTADAQH/MAsGA1UdDwQEAwIBBjANBgkq\n" \
"hkiG9w0BAQsFAAOCAQEAm0/6oaHKBS1hosEdnT5qRT2PzpAsSNAvlU6sCfHbChnO\n" \
"muDeHqQJAPurUkgCRKfGjjCfiG3IJkmFeGlhqWijneII+5MHF/baVMpdx7UejQNO\n" \
"j4WBqWDl6VhKtjdkz7zBEGe7o/V0skSOEc3Xj4+zKMX/swXucggfN1mJmrIFZtLK\n" \
"f3uA3e2tPTNLmEMBAz5X2CkL6dI+/aUDNq75CZ1L9eF92l8F4fqgMjADgETgqHG7\n" \
"z1zWJOHCngVuRzkFkwfpZyItgs7CPBMOJiBlWq0KwMQv2CVmA1BSW9/sPUehnqZQ\n" \
"cn2kLFXZb4N0nS6vgC6kP5fZ9zLsHfDoia4QiIhV5Q==\n" \
"-----END CERTIFICATE-----\n";


const char AWS_REGISTRATION_CLIENT_CERT[] = "-----BEGIN CERTIFICATE-----\n" \
"MIIDvDCCAqSgAwIBAgIQa6vDbLiTui8YUmAtS1bmizANBgkqhkiG9w0BAQsFADAW\n" \
"MRQwEgYDVQQDDAtDQzExQmFkZ2VDQTAeFw0yMzA5MjYwMDQ4MDRaFw0yNTEyMjkw\n" \
"MDQ4MDRaMHkxCzAJBgNVBAYTAlVTMRAwDgYDVQQIDAdBcml6b25hMRAwDgYDVQQH\n" \
"DAdQaG9lbml4MRIwEAYDVQQKDAlDYWN0dXNDb24xDDAKBgNVBAsMA0lPVDEkMCIG\n" \
"A1UEAwwbY2xpZW50cy5iYWRnZS5jYWN0dXNjb24uY29tMIIBIjANBgkqhkiG9w0B\n" \
"AQEFAAOCAQ8AMIIBCgKCAQEA6wjVNVDu7XpVYxqybyp20p/cmMKPDOcpDeiSdc1G\n" \
"rjmuKDhUKxpPgr33Ck5aH6Ip4BvVLPkCj4UwIhxdDNbiU8va6quniNE7LQKXSmMI\n" \
"pU4+P2oBHnx+cDrWGCBSm+k/f39MfaJKLJo2Rv8aJY0g9vZW8LKiN6ncNSLdOWb1\n" \
"zrDayY7elW4xkzFFXIcyz5O97vyWY5nxNbObAMAsARtHV15idc0DNwjTAJZXeRNj\n" \
"Zz2P8oEYOQCI0EIpSCrvRi2DAvENDix1rzQdlxyuJgDL2rPVl8WvQBJld3C7SWCd\n" \
"Yi+lwFnTaBsNJ0i2boNyaPk5/evcAX9rRijIgtmRCrYidwIDAQABo4GiMIGfMAkG\n" \
"A1UdEwQCMAAwHQYDVR0OBBYEFE3zIukR9r/2erimwfXbGVRuXh7UMFEGA1UdIwRK\n" \
"MEiAFOut5l+NrlXiA/Wy3o/NhKkN0tRaoRqkGDAWMRQwEgYDVQQDDAtDQzExQmFk\n" \
"Z2VDQYIUeVE11BE6qdkBOnMRaMgaAzhlLNUwEwYDVR0lBAwwCgYIKwYBBQUHAwIw\n" \
"CwYDVR0PBAQDAgeAMA0GCSqGSIb3DQEBCwUAA4IBAQA4SyoSnKMMT+XArdl3pehe\n" \
"WftzYKY9KDPUOqJ0MK4Z+ZiCRgF9ntOQK4DLxA8GmQX9rgMmAWXamN40iEyQ2YGE\n" \
"/HkcIQuFkU2zw3wFQL35ICJgveiv4uU9jw2AbtqKZwF6Pe/QDUkgZnhZCnu2Nxxj\n" \
"Tw/kYIDrAsR9R2SiXrx3ngw5r4rwXBNtMvmhgu7AyeinePDeIVnMULaflOyA/Ylt\n" \
"chlVJRwFOcU5N/Di0OHbH8BQ170FuyhP39baOg4xu6peJpn1kzt5GV1RS+/rmc40\n" \
"jfJB5VHf6pbCt3JvR01nyONaOoAn+gfNEaGKinnPBzviJj/lyr/RzsYiIt0rY9JI\n" \
"-----END CERTIFICATE-----\n";

const char AWS_REGISTRATION_PRIVATE_KEY[] = "-----BEGIN PRIVATE KEY-----\n" \
"MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQDrCNU1UO7telVj\n" \
"GrJvKnbSn9yYwo8M5ykN6JJ1zUauOa4oOFQrGk+CvfcKTlofoingG9Us+QKPhTAi\n" \
"HF0M1uJTy9rqq6eI0TstApdKYwilTj4/agEefH5wOtYYIFKb6T9/f0x9okosmjZG\n" \
"/xoljSD29lbwsqI3qdw1It05ZvXOsNrJjt6VbjGTMUVchzLPk73u/JZjmfE1s5sA\n" \
"wCwBG0dXXmJ1zQM3CNMAlld5E2NnPY/ygRg5AIjQQilIKu9GLYMC8Q0OLHWvNB2X\n" \
"HK4mAMvas9WXxa9AEmV3cLtJYJ1iL6XAWdNoGw0nSLZug3Jo+Tn969wBf2tGKMiC\n" \
"2ZEKtiJ3AgMBAAECggEAJykUGSuQe7n30yjmFRaesK5ZG4zDN/OmB/65tG4M+kd7\n" \
"CJGGGJ5sJA0RxcJTStUCpCgPB0zAC/i1keFm8ZY2JJ8hLRuHsQinkvLNEWRZ0u0r\n" \
"+vXnFNTKXlm3RVLRYM/muBAWRxKMyEyzgXXMvff7R5q41qb011Byc9oIwSEyEIte\n" \
"UdIZPhhUVFxyv2g1fya1hBLh3s1tkBCU4/P/Verx2l0vhWjCGs6s/OfvlyPgjK+e\n" \
"o0ZibUaLi5DVMGNTvNn/qq8bwK0Xzg5h6REMq2ShwWAoWaa0bRmYg5dDHBiR+W9D\n" \
"v5F+S4Nn5W/FOX3NgMrXYy1iUw+DR8kbkwgDxlXg0QKBgQD4JXw0UT0gD3BstQSA\n" \
"n5a5tMCmNGDfAhF/16/pbd31D5fsJkaT5x7fRWkz1hLV7RuvVTubrAsVB0T4TlXi\n" \
"PzymLj7kNnEpx4AamLNY11FqfEd0kkOus5U3qNGzkd4gwZM/qEYIBK+Wt6GOq8Wd\n" \
"v9RJfG27299bpm8LzqInGByKEQKBgQDyeRz3psX0BGu6ryeF/4wv2cZjy7Hk+lH8\n" \
"e5QOn+SUEc45k3LMxXT+R3gCNxC8ij8O1lWWiO1SQdXkh9V+Tka9GD6Fo+lgcFUH\n" \
"JQZwjQ0zcPhHDLo1QZJ8tFtsftczfidoUD6DYQkUrVe75ipLvmlzgxqdwEeEAZKr\n" \
"pR3CNQycBwKBgQCjMxQS3HMVCNJpzEufxFW+4Q/qv3wOby/PIq2BGk7mA1FpcGru\n" \
"jwyQzve3rc1JgN26XJYNdgL724ISxIJbgDeFG9klAmE+lwS7eT5K6aGUJRRhqGh3\n" \
"F4jFyHPSY9bxOB59uVAFqf9JYpAQYwxfCXC2u+z3pk2p2zYhT875YzQx4QKBgHxA\n" \
"9Ec0f1x8p3gTKf3Q5tXek3uwdokQfdMUwNp9tXTravK2TK4CweFMXoNEFJ+JeKbx\n" \
"zwpFQNhWRHXH5e/DzRw5wFBbnsx8xBXVM2eZ4Jnr2k8Ohp2j7J1bDz6aAU2x6B/c\n" \
"fyx0HWMrMTpoyGPoP8Gh/ZG5ZuyOocqz4nwTE1MhAoGBAJx0NieAQKtix8fkV4W4\n" \
"0WTQrp851/VpTYNoz/tHImoHyzFdbwXx7tQ0s3kX9tOcCoaIfdxORn0/5KaFUzIp\n" \
"PP9ZBCdRLXixKyi3+y0OLfFhLSgqmbhExIahy0lNQQsjmyyTA4Z7SAro953GiXy1\n" \
"v2+CMA5w3x8Q6DuS7Mhv8rTD\n" \
"-----END PRIVATE KEY-----\n";

/**
 * @brief Provides callbacks used for handling various events that can occur in the cc11 badge platform.
*/
class AWSRegistration{
    private:
       private:
        void connect();
    public:
        static bool register_new_device(const char * device_id);
        static bool device_credentials_exist();
        static void load_device_credentials(AWSDeviceCredentials &creds);
        static void clear_saved_credentials();
};
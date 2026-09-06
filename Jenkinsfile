library identifier: 'JenkinsPipelineUtils', changelog: false

withVault([vaultSecrets: [
    [path: 'kv/shared/wifi-iot', engineVersion: 2, secretValues: [
        [envVar: 'WIFI_PASSWORD', vaultKey: 'password'],
    ]],
    [path: 'kv/jenkins/iot-mqtt', engineVersion: 2, secretValues: [
        [envVar: 'MQTT_PASSWORD', vaultKey: 'password'],
    ]],
]]) {
    podTemplate(inheritFrom: 'jenkins-agent-large', containers: [
        containerTemplate(name: 'idf', image: 'espressif/idf:v5.5.3', command: 'sleep', args: 'infinity', envVars: [
            containerEnvVar(key: 'WIFI_PASSWORD', value: '$WIFI_PASSWORD'),
            containerEnvVar(key: 'MQTT_PASSWORD', value: '$MQTT_PASSWORD'),
        ])
    ]) {
        node(POD_LABEL) {
            stage('Build thermostat display') {
                dir('ThermostatDisplay') {
                    git branch: 'main',
                        credentialsId: '5f6fbd66-b41c-405f-b107-85ba6fd97f10',
                        url: 'https://github.com/pvginkel/ThermostatDisplay.git'
                        
                    container('idf') {
                        // Necessary because the IDF container doesn't have support
                        // for setting the uid/gid.
                        sh 'git config --global --add safe.directory \'*\''
                        
                        sh '/opt/esp/entrypoint.sh idf.py build'
                    }
                }
            }
            
            stage('Deploy thermostat display') {
                dir('HelmCharts') {
                    git branch: 'main',
                        credentialsId: '5f6fbd66-b41c-405f-b107-85ba6fd97f10',
                        url: 'https://github.com/pvginkel/HelmCharts.git'
                }

                dir('ThermostatDisplay') {
                    sh 'cp build/esp32-thermostat-display.bin thermostat-display-ota.bin'

                    sh 'scripts/upload.sh ../HelmCharts/assets/kubernetes-signing-key thermostat-display-ota.bin'
                }
            }
        }
    }
}

#include "includes.h"

#include "MQTTConnection.h"

static const char *TAG = "MQTTConnection";
static const char *TOPIC_PREFIX = "pieter";
static const char *DEVICE_MANUFACTURER = "Pieter";
static const char *DEVICE_MODEL = "Thermostat Display";

MQTTConnection::MQTTConnection()
    : _deviceId(getDeviceId()),
      _modeTopic(format("%s/%s/set/mode", TOPIC_PREFIX, CONFIG_DEVICE_NAME)),
      _localTemperatureTopic(format("%s/%s/set/local_temperature", TOPIC_PREFIX, CONFIG_DEVICE_NAME)),
      _setpointTopic(format("%s/%s/set/setpoint", TOPIC_PREFIX, CONFIG_DEVICE_NAME)),
      _stateTopic(format("%s/%s/state", TOPIC_PREFIX, CONFIG_DEVICE_NAME)) {}

void MQTTConnection::begin() {
    esp_mqtt5_connection_property_config_t connect_property = {
        .session_expiry_interval = 10,
        .maximum_packet_size = 1024,
        .receive_maximum = 65535,
        .topic_alias_maximum = 2,
        .request_resp_info = true,
        .request_problem_info = true,
        .will_delay_interval = 10,
        .message_expiry_interval = 10,
        .payload_format_indicator = true,
    };

    const auto lastWillMessage = "offline";

    esp_mqtt_client_config_t configuration = {
        .broker =
            {
                .address =
                    {
                        .uri = CONFIG_MQTT_BROKER_URL,
                    },
            },
        .credentials =
            {
                .username = CONFIG_MQTT_USER_ID,
                .authentication =
                    {
                        .password = CONFIG_MQTT_PASSWORD,
                    },
            },
        .session =
            {
                .last_will =
                    {
                        .topic = _stateTopic.c_str(),
                        .msg = lastWillMessage,
                        .msg_len = strlen(lastWillMessage),
                        .qos = 1,
                        .retain = true,
                    },
                .protocol_ver = MQTT_PROTOCOL_V_5,
            },
        .network =
            {
                .disable_auto_reconnect = false,
            },
    };

    _client = esp_mqtt_client_init(&configuration);

    esp_mqtt5_client_set_connect_property(_client, &connect_property);

    esp_mqtt_client_register_event(_client, MQTT_EVENT_ANY, eventHandlerThunk, this);
    esp_mqtt_client_start(_client);
}

string MQTTConnection::getDeviceId() {
    uint8_t mac[6];

    ESP_ERROR_CHECK(esp_read_mac(mac, ESP_MAC_WIFI_STA));

    return format("0x%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void MQTTConnection::eventHandler(esp_event_base_t eventBase, int32_t eventId, void *eventData) {
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32, eventBase, eventId);
    auto event = (esp_mqtt_event_handle_t)eventData;

    ESP_LOGD(TAG, "Free heap size is %" PRIu32 ", minimum %" PRIu32, esp_get_free_heap_size(),
             esp_get_minimum_free_heap_size());

    switch ((esp_mqtt_event_id_t)eventId) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT connected");
            handleConnected();
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT disconnected");
            break;

        case MQTT_EVENT_SUBSCRIBED:
            break;

        case MQTT_EVENT_UNSUBSCRIBED:
            break;

        case MQTT_EVENT_PUBLISHED:
            break;

        case MQTT_EVENT_DATA:
            handleData(event);
            break;

        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT return code is %d", event->error_handle->connect_return_code);
            if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
                if (event->error_handle->esp_tls_last_esp_err) {
                    ESP_LOGI(TAG, "reported from esp-tls");
                }
                if (event->error_handle->esp_tls_stack_err) {
                    ESP_LOGI(TAG, "reported from tls stack");
                }
                if (event->error_handle->esp_transport_sock_errno) {
                    ESP_LOGI(TAG, "captured as transport's socket errno");
                }
                ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
            }
            break;

        default:
            ESP_LOGD(TAG, "Other event id: %d", event->event_id);
            break;
    }
}

void MQTTConnection::handleConnected() {
    subscribe(_modeTopic);
    subscribe(_localTemperatureTopic);
    subscribe(_setpointTopic);

    publishDiscovery();
    setOnline();

    /*
    print_user_property(event->property->user_property);
    esp_mqtt5_client_set_user_property(&publish_property.user_property, user_property_arr, USE_PROPERTY_ARR_SIZE);
    esp_mqtt5_client_set_publish_property(_client, &publish_property);
    msg_id = esp_mqtt_client_publish(_client, "/topic/qos1", "data_3", 0, 1, 1);
    esp_mqtt5_client_delete_user_property(publish_property.user_property);
    publish_property.user_property = NULL;
    ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);

    esp_mqtt5_client_set_user_property(&subscribe_property.user_property, user_property_arr, USE_PROPERTY_ARR_SIZE);
    esp_mqtt5_client_set_subscribe_property(_client, &subscribe_property);
    msg_id = esp_mqtt_client_subscribe(_client, "/topic/qos0", 0);
    esp_mqtt5_client_delete_user_property(subscribe_property.user_property);
    subscribe_property.user_property = NULL;
    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

    esp_mqtt5_client_set_user_property(&subscribe1_property.user_property, user_property_arr, USE_PROPERTY_ARR_SIZE);
    esp_mqtt5_client_set_subscribe_property(_client, &subscribe1_property);
    msg_id = esp_mqtt_client_subscribe(_client, "/topic/qos1", 2);
    esp_mqtt5_client_delete_user_property(subscribe1_property.user_property);
    subscribe1_property.user_property = NULL;
    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);

    esp_mqtt5_client_set_user_property(&unsubscribe_property.user_property, user_property_arr, USE_PROPERTY_ARR_SIZE);
    esp_mqtt5_client_set_unsubscribe_property(_client, &unsubscribe_property);
    msg_id = esp_mqtt_client_unsubscribe(_client, "/topic/qos0");
    ESP_LOGI(TAG, "sent unsubscribe successful, msg_id=%d", msg_id);
    esp_mqtt5_client_delete_user_property(unsubscribe_property.user_property);
    unsubscribe_property.user_property = NULL;
    */
}

void MQTTConnection::handleData(esp_mqtt_event_handle_t event) {
    /*
    ESP_LOGI(TAG, "MQTT_EVENT_DATA");
    print_user_property(event->property->user_property);
    ESP_LOGI(TAG, "payload_format_indicator is %d", event->property->payload_format_indicator);
    ESP_LOGI(TAG, "response_topic is %.*s", event->property->response_topic_len, event->property->response_topic);
    ESP_LOGI(TAG, "correlation_data is %.*s", event->property->correlation_data_len, event->property->correlation_data);
    ESP_LOGI(TAG, "content_type is %.*s", event->property->content_type_len, event->property->content_type);
    ESP_LOGI(TAG, "TOPIC=%.*s", event->topic_len, event->topic);
    ESP_LOGI(TAG, "DATA=%.*s", event->data_len, event->data);
    */
}

void MQTTConnection::subscribe(string &topic) {
    ESP_LOGI(TAG, "Subscribing to topic %s", topic.c_str());

    ESP_ERROR_ASSERT(esp_mqtt_client_subscribe(_client, topic.c_str(), 0) >= 0);
}

void MQTTConnection::publishDiscovery() {
    ESP_LOGI(TAG, "Publishing discovery information");

    auto topic = format("%s/%s", TOPIC_PREFIX, CONFIG_DEVICE_NAME);
    auto stateTopic = format("%s/%s/state", TOPIC_PREFIX, CONFIG_DEVICE_NAME);
    auto uniqueIdentifier = format("%s_%s", TOPIC_PREFIX, _deviceId.c_str());

    auto root = cJSON_CreateObject();

    auto availabilityTopic = cJSON_CreateObject();
    cJSON_AddStringToObject(availabilityTopic, "topic", stateTopic.c_str());

    auto availability = cJSON_AddArrayToObject(root, "availability");
    cJSON_AddItemToArray(availability, availabilityTopic);

    cJSON_AddStringToObject(root, "action_template", "{{ value_json.state }}");
    cJSON_AddStringToObject(root, "action_topic", topic.c_str());
    cJSON_AddStringToObject(root, "current_temperature_template", "{{ value_json.local_temperature }}");
    cJSON_AddStringToObject(root, "current_temperature_topic", topic.c_str());

    auto device = cJSON_AddObjectToObject(root, "device");

    auto identifiers = cJSON_AddArrayToObject(device, "identifiers");
    cJSON_AddItemToArray(identifiers, cJSON_CreateString(uniqueIdentifier.c_str()));

    cJSON_AddStringToObject(device, "manufacturer", DEVICE_MANUFACTURER);
    cJSON_AddStringToObject(device, "model", DEVICE_MODEL);
    cJSON_AddStringToObject(device, "name", CONFIG_DEVICE_NAME);

    cJSON_AddStringToObject(root, "device_class", "temperature");
    cJSON_AddNumberToObject(root, "max_temp", 35);
    cJSON_AddNumberToObject(root, "min_temp", 4);
    cJSON_AddStringToObject(root, "mode_command_topic", _modeTopic.c_str());
    cJSON_AddStringToObject(root, "mode_state_template", "{{ value_json.mode }}");
    cJSON_AddStringToObject(root, "mode_state_topic", topic.c_str());

    auto modes = cJSON_AddArrayToObject(root, "modes");
    cJSON_AddItemToArray(modes, cJSON_CreateString("off"));
    cJSON_AddItemToArray(modes, cJSON_CreateString("heat"));

    cJSON_AddNullToObject(root, "name");
    cJSON_AddStringToObject(root, "object_id", CONFIG_DEVICE_ENTITY_ID);
    cJSON_AddNumberToObject(root, "temp_step", 0.5);
    cJSON_AddStringToObject(root, "temperature_command_topic", _setpointTopic.c_str());
    cJSON_AddStringToObject(root, "temperature_state_template", "{{ value_json.setpoint }}");
    cJSON_AddStringToObject(root, "temperature_state_topic", topic.c_str());
    cJSON_AddStringToObject(root, "temperature_unit", "C");
    cJSON_AddStringToObject(root, "unique_id", uniqueIdentifier.c_str());

    auto json = cJSON_Print(root);

    auto publishTopic = format("homeassistant/climate/%s/climate/config", _deviceId.c_str());

    ESP_ERROR_ASSERT(esp_mqtt_client_publish(_client, publishTopic.c_str(), json, 0, 0, true) >= 0);

    cJSON_free(json);
}

void MQTTConnection::setOnline() {
    ESP_ERROR_ASSERT(esp_mqtt_client_publish(_client, _stateTopic.c_str(), "online", 0, 0, false) >= 0);
}

#define STR1 "{\"fields\": {\"value\":{\""
#define STR2 "\": \""
#define STR3 "\"},\"sensorId\":{\"stringValue\":\""
#define STR4 "\"},\"createdAt\":{\"timestampValue\": \""
#define STR5 "\" }}}"


// make a json string
String makeJson(String dataType, String sensorData, String timeStamp, String sensor_ID){
  String jsonData = STR1 + dataType + STR2 + sensorData + STR3 + sensor_ID + STR4 + timeStamp + STR5;
  return jsonData;
}

String httpGETRequest(const char * serverName) { //function to call GET Requests 
  HTTPClient http;
  http.setReuse(true);
  http.setTimeout(99000);
  http.setConnectTimeout(99000);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.begin(serverName);

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode == HTTP_CODE_OK) {
    //Serial.print("HTTP Response code: ");
    //Serial.println(httpResponseCode);
    payload = http.getString();
  } else {
    payload = http.getString();

    Serial.printf("Error code: %s %s  -  %s \n", http.getString(), httpResponseCode, http.errorToString(httpResponseCode).c_str());

  }
  // Free resources
  http.end();

  return payload;
}

String session_key = "Saj2UmOZRdwtJvU";
String ip = "10.0.4.83";
String create_game_api = "http://10.0.4.83:5000/api/create_game"; //?key={session}&time=10-0&username=test_api
String get_fen_api = "http://10.0.4.83:5000/api/fen"; //?key={session}&gameid={gameid}
String is_gameover_api = "http://10.0.4.83:5000/api/gameover"; //?key={session}&gameid={gameid}
String is_myturn_api = "http://10.0.4.83:5000/api/myturn"; //?key={session}&gameid={gameid}
String make_move_api = "http://10.0.4.83:5000/api/makemove"; //?key={session}&gameid={gameid}&color={color}&start={start}&end={end}
String create_session_api = "http://10.0.4.83:5000/api/create_session";

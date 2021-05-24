#include "ChessDotComLib.h"

ChessCom instance;

void benchmark() { //function to benchmark api calls speed.
    int counter = 0;
    while (1) {
        counter += 1;
        Serial.println(counter);
        Serial.println(httpGETRequest("http://10.100.0.73:5000/api/fen?key=D7Ip3ROLKKcMz8M&gameid=14040703649"));
        delay(0);
    }
}

String input(String msg, CreateGame instance) { //function to take an input from serial
    Serial.println(msg);
    bool typed = false;
    String response;
    while (!typed) {
        if (instance.gameover_store) {
            typed = true;
            return "";
        }
        if (Serial.available() > 0) {
            String incomingByte = Serial.readString();
            response = incomingByte;
            response.trim();
            typed = true;
            return response;
        }
    }

}

int ChessComLogin() { //function to login using chessdotcomlib. WIP. This should take an input.
    instance.createSession("Noobist_007", "bastienRES13"); //fake account btw

    Serial.printf("Successfully Logged in as: '%s'\n Session_ID: '%s'\n",instance.username, instance.session);

    return 0;
}

int ChessComCreateGame() {//create a game using chessdotcomlib. WIP. Should take an input.
    CreateGame ChessGame = CreateGame("10-0", instance);

    ChessGame.startsync(); //start background job, to check for game updates in the background.

    Serial.printf("Successfully Created Game [%s] as: '%s' \n",ChessGame.gameid, instance.username);
    Serial.printf("The fen is: %s \n",ChessGame.fen_store);
    
    delay(1000);
    if (!ChessGame.gameover_store) {
        Serial.printf("FEN: %s \n",ChessGame.fen_store);
        Serial.printf("MyTurn: %s \n",ChessGame.myturn_store ? "true" : "false");
        Serial.printf("Gameover: %s \n\n",ChessGame.gameover_store ? "true" : "false");


        delay(1000);
        while (!ChessGame.gameover_store) { //infinite loop. This makes sure our device doesn't get OTA update from main loop.
            delay(1000);                    // The game handler is also implemented in the loop
            if (ChessGame.myturn_store) {
                String startsquare = input("Enter Start Square:", ChessGame); //Should Take Input
                String endsquare = input("Enter End Square:", ChessGame); //Should take input.
                if (!ChessGame.gameover_store) {
                    if (ChessGame.make_move(startsquare, endsquare)) {
                        Serial.printf("Successfully played %s-%s \n",startsquare,endsquare);
                        
                    } else {
                        Serial.printf("Error Playing %s-%s \n",startsquare,endsquare);
                    }
                }
            }

        }
    }

    ChessGame.stopsync(); //stop background job

    Serial.println("Gameover: TRUE");
    return 0;
}

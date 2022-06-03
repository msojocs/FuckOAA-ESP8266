#include "server.h"
ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/html", "<h1>You are connected</h1>");
}

void Server_Start()
{
    server.on("/", handleRoot);
    server.begin();
}
void Server_HandleClient()
{
    server.handleClient();
}
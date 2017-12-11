#include <MeteoData.h>

MeteoData::MeteoData(){
}

int MeteoData::convertWindDirection(const char* direction){
  const char S[] = "Pietu0173";
  const char N[] = "u0160iauru0117s";
  const char E[] = "Rytu0173";
  const char W[] = "Vakaru0173";

  const char NE[] = "u0160iauru0117s rytu0173";
  const char SE[] = "Pietryu010diu0173";
  const char SW[] = "Pietvakariu0173";
  const char NW[] = "u0160iauru0117s vakaru0173";

  if(strcmp(direction, S) == 0) { return 180;}
  if(strcmp(direction, N) == 0) { return 0;}
  if(strcmp(direction, E) == 0) { return 90;}
  if(strcmp(direction, W) == 0) { return 270;}
  if(strcmp(direction, NE) == 0) { return 45;}
  if(strcmp(direction, SE) == 0) { return 135;}
  if(strcmp(direction, SW) == 0) { return 225;}
  if(strcmp(direction, NW) == 0) { return 315;}
  return 0;
}

void MeteoData::formatUrl(unsigned int source, int id){

  char id_str[5];
  itoa(id, id_str, 10);

  if (source == SOURCE_VU){
    //const char url[] = "GET /json.php HTTP/1.1";
    url[0] = '\0';
    strcat(url, "GET ");
    strcat(url, url_VU);
    strcat(url, " HTTP/1.1");
    Serial.println(url);
  }

  if (source == SOURCE_KD){
    //const char url[] = "GET /weather-conditions-retrospective?id=1166&number=1 HTTP/1.1";
    url[0] = '\0';
    strcat(url, "GET ");
    strcat(url, url_KD);
    strcat(url, id_str);
    strcat(url, "&number=1 HTTP/1.1");
    Serial.println(url);
  }

  return;
}

void MeteoData::read(int source, int id){

  WiFiClient client;
  char jsonData[1024];
  const char *host;

  //set JSON buffer
  StaticJsonBuffer<1024> jsonBuffer;

  formatUrl(source, id);

  if (source == SOURCE_VU){
    datastatus = client.connect(host_VU, port);
    host = host_VU;
  }
  if (source == SOURCE_KD){
    datastatus = client.connect(host_KD, port);
    host = host_KD;
  }

  delay(500);

  if (datastatus){
      Serial.println("Connected");
      client.print(url);
      client.print("\r\n");
      client.print("Host: ");
      client.print(host);
      client.print("\r\n");
      client.print("Connection: close");
      client.print("\r\n\r\n");
      delay(100);
      //================KD data parse================
      if (source == SOURCE_KD){
        if (client.find("[")) {
          Serial.println("Data start OK");
          int i = 0;
          while (client.available()) {
            jsonData[i] = client.read();
            i++;
          }

          JsonObject& root = jsonBuffer.parseObject(jsonData);

          if (!root.success()) {
            error_code = PARSE_ERROR;
            return;
          }

          temperature = root["oro_temperatura"];
          windspeed = root["vejo_greitis_vidut"];
          winddirection = convertWindDirection(root["vejo_kryptis"]);
          //dewpoint = root["rasos_taskas"];
          //humidity = getHumidity(temperature, dewpoint);
          Serial.print("Temperature: ");
          Serial.println(temperature);
          Serial.print("Windspeed: ");
          Serial.println(windspeed);
          Serial.print("Humidity: ");
          Serial.println(humidity);
          Serial.print("Direction: ");
          Serial.println(winddirection);
        } else {
          error_code = HEADER_ERROR;
          return;
        }
      }

      //================VU data parse================
      if (source == SOURCE_VU){

          if (client.find("wup(")) {
            Serial.println("Data start OK");
            int i = 0;
            while (client.available()) {
              jsonData[i] = client.read();
              i++;
            }

            JsonObject& root = jsonBuffer.parseObject(jsonData);

            if (!root.success()) {
              error_code = PARSE_ERROR;
              return;
            }

            temperature = root["zeno_AT_5s_C"];
            windspeed = root["zeno_gust"];
            winddirection = root["zeno_Dir_5s"];
            humidity = root["zeno_RH_5s"];

            Serial.print("Temperature: ");
            Serial.println(temperature);
            Serial.print("Windspeed: ");
            Serial.println(windspeed);
            Serial.print("Humidity: ");
            Serial.println(humidity);
            Serial.print("Direction: ");
            Serial.println(winddirection);
          } else {
            error_code = HEADER_ERROR;
            return;
          }
      }

  } else {
    error_code = HOST_ERROR;
    return;
  }
}

float MeteoData::getHumidity(float T, float TD) {

  float hum = 100 * pow((112 - 0.1 * T + TD) / (112 + 0.9 * T), 8);

  if (hum > 99 ){
    hum = 99;
  }
  return hum;
}

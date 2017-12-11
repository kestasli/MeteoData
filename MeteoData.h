#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#define SOURCE_VU 0 //Vilnius
#define SOURCE_KD 1 //9 fortas

#define HOST_ERROR 0;
#define HEADER_ERROR 1;
#define PARSE_ERROR 2;

class MeteoData{
public:
  MeteoData();
  void read(int source, int id);
  float temperature;
  float windspeed;
  int winddirection;
  int humidity;
  float pressure;
private:
  int convertWindDirection(const char* direction);
  float getHumidity(float T, float TD);
  const int port = 80;
  const char *host_VU = "www.hkk.gf.vu.lt";
  const char *url_VU = "/json.php";
  const char *host_KD = "eismoinfo.lt";
  const char *url_KD = "/weather-conditions-retrospective?id=";
  char url[128];
  void formatUrl(unsigned int source, int id);
  bool datastatus; //true if no errors encountered, false if errors
  int error_code;
};

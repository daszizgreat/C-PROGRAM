#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

// Callback function for curl
size_t write_callback(char *ptr, size_t size, size_t nmemb, char **data)
{
    size_t realsize = size * nmemb;
    *data = realloc(*data, realsize + 1);
    if (*data == NULL) {
        printf("Memory allocation failed.\n");
        return 0;
    }
    memcpy(*data, ptr, realsize);
    (*data)[realsize] = '\0';
    return realsize;
}

int main()
{
    CURL *curl;
    CURLcode res;
    char *data = NULL;
    cJSON *root, *weather, *weather_desc, *main_info, *temperature, *humidity;
    char *city = "YOUR_CITY"; // Replace with your city name
    char url[100];

    // Create URL for API request
    sprintf(url, "http://api.openweathermap.org/data/2.5/weather?q=%s&appid=YOUR_API_KEY", city); // Replace with your API key

    // Initialize CURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        // Set API URL
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Set callback function to retrieve response data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return 1;
        }

        // Parse JSON response
        root = cJSON_Parse(data);
        if (root == NULL) {
            printf("Failed to parse JSON data.\n");
            return 1;
        }

        // Retrieve weather information
        weather = cJSON_GetObjectItemCaseSensitive(root, "weather");
        weather_desc = cJSON_GetObjectItemCaseSensitive(cJSON_GetArrayItem(weather, 0), "description");
        main_info = cJSON_GetObjectItemCaseSensitive(root, "main");
        temperature = cJSON_GetObjectItemCaseSensitive(main_info, "temp");
        humidity = cJSON_GetObjectItemCaseSensitive(main_info, "humidity");

        // Display weather information
        printf("City: %s\n", city);
        printf("Weather: %s\n", cJSON_GetStringValue(weather_desc));
        printf("Temperature: %.2f degrees Celsius\n", cJSON_GetNumberValue(temperature) - 273.15);
        printf("Humidity: %d%%\n", cJSON_GetNumberValue(humidity));

        // Clean up
        cJSON_Delete(root);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        free(data);
    }

    return 0;
}

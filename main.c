#include "json/json.h"
#include "fmt/fmt.h"

int main() {
    const char* jsonString = 
        "{"
        "  \"technology\": {"
        "    \"products\": ["
        "      {"
        "        \"category\": \"Laptops\","
        "        \"items\": ["
        "          {"
        "            \"name\": \"Laptop A\","
        "            \"brand\": \"BrandOne\","
        "            \"price\": 1200,"
        "            \"ratings\": {"
        "              \"tech_site\": 9.1,"
        "              \"user_reviews\": 8.5"
        "            }"
        "          },"
        "          {"
        "            \"name\": \"Laptop B\","
        "            \"brand\": \"BrandTwo\","
        "            \"price\": 1500,"
        "            \"ratings\": {"
        "              \"tech_site\": 9.3,"
        "              \"user_reviews\": 9.0"
        "            }"
        "          }"
        "        ]"
        "      },"
        "      {"
        "        \"category\": \"Smartphones\","
        "        \"items\": ["
        "          {"
        "            \"name\": \"Smartphone A\","
        "            \"brand\": \"BrandThree\","
        "            \"price\": 700,"
        "            \"ratings\": {"
        "              \"tech_site\": 8.5,"
        "              \"user_reviews\": 8.8"
        "            }"
        "          },"
        "          {"
        "            \"name\": \"Smartphone B\","
        "            \"brand\": \"BrandFour\","
        "            \"price\": 950,"
        "            \"ratings\": {"
        "              \"tech_site\": 9.0,"
        "              \"user_reviews\": 9.2"
        "            }"
        "          }"
        "        ]"
        "      }"
        "    ]"
        "  }"
        "}";

    JsonElement* root = json_parse(jsonString);
    JsonElement* laptopAPriceElement = json_query(root, "technology.products[0].items[0].price");

    if (laptopAPriceElement && laptopAPriceElement->type == JSON_NUMBER) {
        fmt_printf("Price of Laptop A: %.2f\n", laptopAPriceElement->value.number_val);
    } 
    else {
        fmt_printf("Failed to query the price of Laptop A.\n");
    }

    JsonElement* smartphoneBRatingElement = json_query(root, "technology.products[1].items[1].ratings.tech_site");
    if (smartphoneBRatingElement && smartphoneBRatingElement->type == JSON_NUMBER) {
        fmt_printf("Tech site rating of 'Smartphone B': %.1f\n", smartphoneBRatingElement->value.number_val);
    } 
    else {
        fmt_printf("Failed to query the tech site rating of 'Smartphone B'.\n");
    }

    json_deallocate(root);
    return 0;
}
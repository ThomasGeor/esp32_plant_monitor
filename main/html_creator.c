#include <html_creator.h>

void create_webpage(char *html_code)
{
  sprintf(html_code, "<!DOCTYPE html>\n"
                     "<html lang=\"en\">\n"
                     "  <head>\n"
                     "    <meta charset=\"UTF-8\" />\n"
                     "    <meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\" />\n"
                     "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\" />\n"
                     "    <link\n"
                     "      href=\"https://fonts.googleapis.com/css2?family=Inter:wght@400;500;600;700&display=swap\"\n"
                     "      rel=\"stylesheet\"\n"
                     "    />\n"
                     "    <title>Plant Monitor</title>\n"
                     "    <style>\n"
                     "      * {\n"
                     "        margin: 0;\n"
                     "        padding: 0;\n"
                     "        box-sizing: border-box;\n"
                     "      }\n"
                     "      body {\n"
                     "        font-family: \"Inter\", sans-serif;\n"
                     "        color: #343a40;\n"
                     "        line-height: 1;\n"
                     "        display: flex;\n"
                     "        justify-content: center;\n"
                     "      }\n"
                     "\n"
                     "      table {\n"
                     "        width: 500px;\n"
                     "        margin-top: 100px;\n"
                     "        font-size: 18px;\n"
                     "        border-collapse: collapse;\n"
                     "      }\n"
                     "      td,\n"
                     "      th {\n"
                     "        padding: 16px 24px;\n"
                     "        text-align: left;\n"
                     "      }\n"
                     "\n"
                     "      thead th {\n"
                     "        background-color: #087f5b;\n"
                     "        color: white;\n"
                     "        width: 25%%;\n"
                     "      }\n"
                     "\n"
                     "      tbody tr:nth-child(odd) {\n"
                     "        background-color: #f8f9fa;\n"
                     "      }\n"
                     "      tbody tr:nth-child(even) {\n"
                     "        background-color: #e9ecef;\n"
                     "      }\n"
                     "    </style>\n"
                     "  </head>\n"
                     "  <body>\n"
                     "    <table>\n"
                     "      <thead>\n"
                     "        <tr>\n"
                     "          <th>Plant Sensors</th>\n"
                     "          <th>Value (SI)</th>\n"
                     "        </tr>\n"
                     "      </thead>\n"
                     "      <tbody>\n"
                     "        <tr>\n"
                     "          <th>TC74 Temperature</th>\n"
                     "          <td>%d C°</td>\n"
                     "        </tr>\n"
                     "        <tr>\n"
                     "          <th>MQ135 CO2 Level</th>\n"
                     "          <td>%d PPM</td>\n"
                     "        </tr>\n"

                     "      </tbody>\n"
                     "    </table>\n"
                     "  </body>\n"
                     "</html>",
          temperature_reading(), 0);
}
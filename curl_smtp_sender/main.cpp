#include <stdio.h>
#include <curl/curl.h>
#include "glib.h"
#include <string>
#include "gmime/gmime.h"
#include "gobject/gobject.h"
#include <netinet/in.h>
#include <netdb.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <getopt.h>

#define BUFFEr_SIZE 4096

using namespace std;

#define FROM    "<amd0201@aol.com>"
#define TO      "<amd0201@aol.com>"

int send_mail(const char *to, const char *from, const char *password, const char *smtp_url, const char *subject, const char *body) {
    CURL *curl;
    CURLcode curl_code;
    struct curl_slist *recipients = NULL;
    struct curl_slist *slist = NULL;
    struct curl_slist *headers = NULL;
    curl_mime *mime;
    curl_mime *alt;
    curl_mimepart *part;
    const char **cpp;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_USERNAME, from);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password);
        curl_easy_setopt(curl, CURLOPT_URL, smtp_url);  //smtp.gmail.com:587
        recipients = curl_slist_append(recipients, to);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from);
        curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
        
        static const char *headers_text[] = {
        "Date: Tue, 22 Aug 2017 14:08:43 +0100",
        "To: " TO,
        "From: " FROM " (Alex Davenport)",
        "Message-ID: <dcd7cb36-11wb-487a-9f3a-e652a9458efd@"
          "rfcpedant.example.org>",
        "Subject: example sending a MIME-formatted message",
        NULL
      };
        
        /* Build and set the message header list. */ 
        for(cpp = headers_text; *cpp; cpp++)
        headers = curl_slist_append(headers, *cpp);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        /* Build the mime message. */ 
        mime = curl_mime_init(curl);

        /* The inline part is an alterative proposing the html and the text
        versions of the e-mail. */ 
        alt = curl_mime_init(curl);

        /* Text message. */ 
        part = curl_mime_addpart(alt);
        curl_mime_data(part, body, CURL_ZERO_TERMINATED);

        /* Create the inline part. */ 
        part = curl_mime_addpart(mime);
        curl_mime_subparts(part, alt);
        curl_mime_type(part, "multipart/alternative");
        slist = curl_slist_append(NULL, "Content-Disposition: inline");
        curl_mime_headers(part, slist, 1);        

        curl_code = curl_easy_perform(curl);

        /* Free lists. */ 
        curl_slist_free_all(recipients);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    return curl_code;
}
 
 
int main(int argc, char **argv){

    char *to; char *from; char *password; char *smtp_url; char *subject; char *body; char *cc;
    
    int c;
    
    while (1) {

        /* all require arguments if they are going to be included, 
         * otherwise, blank is entered */
        static struct option long_options[] = {
            {"to",     	required_argument,  0, 't'},
            {"from",    required_argument,  0, 'f'},
            {"password",required_argument,  0, 'p'},
            {"cc",      required_argument,  0, 'c'},
            {"bcc",  	required_argument,  0, 'x'},
            {"url",  	required_argument,  0, 'u'},
            {"subject", required_argument,  0, 's'},
            {"body",    required_argument,  0, 'b'},
            {"version",	no_argument,        0, 'v'},
            {"help",    no_argument,        0, 'h'},
            {0, 0, 0, 0}
        };

        /* getopt_long stores the option index here. */
        int option_index = 0;

        c = getopt_long (argc, argv, "t:f:p:c:x:u:s:b:vh", long_options, &option_index);

        /* Detect the end of the options. */
        if (c == -1)
                break;

        switch (c) {
            case 0:
                printf ("option %s", long_options[option_index].name);
                if (optarg)
                        printf (" with arg %s", optarg);
                printf ("\n");
                break;
            case 't':
                printf ("option -t with value '%s'\n", optarg);
                to = optarg;
                break;
            case 'f':
                printf ("option -f with value '%s'\n", optarg);
                from = optarg;
                break;
            case 'p':
                printf ("option -p with value '%s'\n", optarg);
                password = optarg;
                break;
            case 'c':
                printf ("option -c with value '%s'\n", optarg);
                cc = optarg;
                break;
            case 'x':
                printf ("option -x with value '%s'\n", optarg);
                break;
            case 'u':
                printf ("option -u with value '%s'\n", optarg);
                smtp_url = optarg;
                break;
            case 's':
                printf ("option -s with value '%s'\n", optarg);
                subject = optarg;
                break;
            case 'b':
                printf ("option -b with value '%s'\n", optarg);
                body = optarg;
                break;
            case 'v':
                puts ("outputting version info here");
                break;
            case 'h':
                puts ("outputting help info here");
                break;
            case '?':
                /* getopt_long already printed an error message. */
                break;

            default:
                abort ();
        }
    }

    /* Print any remaining command line arguments (not options). */
    if (optind < argc) {
            printf ("non-option ARGV-elements: ");
            while (optind < argc)
                    printf ("%s ", argv[optind++]);
            putchar ('\n');
    }

    send_mail(to, from, password, smtp_url, subject, body);
       
    return 0;
}

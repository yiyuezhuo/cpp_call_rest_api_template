#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

#include <fstream>
#include <iostream>

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
// https://github.com/Tencent/rapidjson

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;
using namespace rapidjson;


static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

string send_char_arr(char * memblock, long size){
    CURL *curl;
    CURLcode res;

    string readBuffer;

    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:4698/predict");

        // disable Expect:
        struct curl_slist *chunk = NULL;
        chunk = curl_slist_append(chunk, "Content-Type: opencv/image"); // use strange type to prevent server to parse content
        chunk = curl_slist_append(chunk, "Expect:");
        res = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, chunk);

        // data
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, memblock);
        // data length
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, size);

        // response handler
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

        /* Perform the request, res will get the return code */ 
        res = curl_easy_perform(curl);

        /* Check for errors */ 
        if(res != CURLE_OK){
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
        }

        /* always cleanup */ 
        curl_easy_cleanup(curl);
    }

    //delete[] memblock; // memblock should be freed from called here.
    return readBuffer;
}

string send_mat(Mat mat){
    vector<uchar> buf;
    imencode(".png", mat, buf);
    char * memblock = reinterpret_cast<char*>(buf.data());
    long size = buf.size();

    return send_char_arr(memblock, size);
}

int main(void){
    Mat mat = imread("decoded.png");
    string resp = send_mat(mat);
    
    cout << "resp Begin:" << endl;
    cout << resp << endl;
    cout << "resp End" << endl;

    Document d;
    d.Parse(resp.c_str());
    int class_id = d["class_id"].GetInt();
    string class_name = d["class_name"].GetString();

    cout << "class_id: " << class_id << " class_name: " << class_name << endl;

    cout << "end" << endl;
    return 0;
}
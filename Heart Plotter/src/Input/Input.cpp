#include "Input.h"
#include <iostream>
#include <fstream>
#include <string> 
#include <wfdb.h>


OperationStatus load_data(std::string record){
    WFDB_Sample v[2];
    WFDB_Siginfo s[2];
    
    // Ustawienie początkowej wartości statusu na Failure
    OperationStatus status = OperationStatus::ERROR;

    // Przykładowe wczytywanie danych
    if (isigopen(const_cast<char*>(record.c_str()), s, 2) < 2) {
        // W przypadku niepowodzenia ustawienie statusu na Failure
        std::cerr << "Błąd przy otwieraniu sygnału!" << std::endl;
    } else {
        // W przypadku powodzenia ustawienie statusu na Success
        status = OperationStatus::SUCCESS;
        std::cout << "Dane wczytane poprawnie." << std::endl;
        // Tutaj możesz dodać kod przetwarzający wczytane dane
    }

    // Zwrócenie statusu operacji
    return status;
};



std::vector<DataPoint> get_preprocessed_data(std::string filename)
{
    int i;
    int nr_samples;
    WFDB_Sample v[2];
    WFDB_Siginfo s[2];

    if (isigopen(const_cast<char*> (filename.c_str()), s, 2) < 2)
        exit(1);
    nr_samples = s[1].nsamp; 

    // Vector do przechowywania punktów danych
    std::vector<DataPoint> punktyDanych;

    for (i = 0; i < nr_samples; i++) {
        if (getvec(v) < 0)
            break;

        // Tworzenie nowego punktu danych i dodawanie go do wektora
        DataPoint nowyPunkt(double(i) / 360, v[1]);  
        punktyDanych.push_back(nowyPunkt);
    }
    return punktyDanych;
};







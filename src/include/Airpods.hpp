#ifndef AIRPODS_H
#define AIRPODS_H

#define MIN_RSSI -76
#define AIRPODS_MANUFACTURER 76
#define AIRPODS_DATA_LENGTH 27
#define AIRPODS_LEFT_BATTERY 18
#define AIRPODS_RIGHT_BATTERY 19
#define AIRPODS_CASE_BATTERY 18

#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#include <simpleble/Adapter.h>
#include <simpleble/Peripheral.h>

struct Airpods
{
    Airpods()
    {
        if (!checkValidity())
        {
            return;
        }
        m_firstAdapter.scan_for(2000);
        std::vector<SimpleBLE::Peripheral> peripherals = m_firstAdapter.scan_get_results();
        for (auto el : peripherals)
        {
            if (!checkIfPeripheralIsPod(el))
            {
                continue;
            };
            std::cout << el.identifier() << std::endl;
            SimpleBLE::ByteArray data = el.manufacturer_data()[AIRPODS_MANUFACTURER];
            if (data.length() != AIRPODS_DATA_LENGTH)
            {
                continue;
            }
            hexData = hexStr(std::move(data));
            std::cout << " | " << hexData << " | " << std::endl;
            checkIfFlipped();
            short int lBattery = std::stoul(std::string{hexData[m_is_flipped ? AIRPODS_LEFT_BATTERY : AIRPODS_RIGHT_BATTERY]}, nullptr, 16) * 10;
            lBattery = lBattery > 100 ? -1 : lBattery;
            short int rBattery = std::stoul(std::string{hexData[m_is_flipped ? AIRPODS_RIGHT_BATTERY : AIRPODS_LEFT_BATTERY]}, nullptr, 16) * 10;
            rBattery = rBattery > 100 ? -1 : rBattery;
            short int cBattery = std::stoul(std::string{hexData[27]}, nullptr, 16) * 10;
            cBattery = cBattery > 100 ? -1 : cBattery;
            std::string modelName;
            switch (hexData[7])
            {
            case '3':
                modelName = "AirPods 3th gen";
                break;
            case '2':
                modelName = "AirPods 1st gen";
                break;
            case 'f':
                modelName = "AirPods 2nd gen";
                break;
            case 'a':
                modelName = "AirPods Max";
                break;
            case 'e':
                modelName = "AirPods Pro";
                break;
            default:
                modelName = "unknown";
                break;
            }
            std::cout << "Model : " << modelName << std::endl;
            std::cout << "lBattery : " << lBattery << std::endl;
            std::cout << "rBattery : " << rBattery << std::endl;
            std::cout << "cBattery : " << cBattery << std::endl;
        }
    }

private:
    bool m_charging_left = false;
    bool m_charging_right = false;
    bool m_charging_case = false;
    SimpleBLE::Adapter m_firstAdapter;
    std::string hexData;
    bool m_is_flipped = false;

    std::string hexStr(SimpleBLE::ByteArray &&data)
    {
        std::stringstream ss;
        ss << std::hex;
        for (auto c : data)
        {
            ss << std::setw(2) << std::setfill('0') << (int)c;
        }
        return ss.str();
    }
    void checkIfFlipped()
    {
        m_is_flipped = hexData[10] & 0x02 == 0;
    }
    bool checkValidity()
    {
        if (!SimpleBLE::Adapter::bluetooth_enabled())
        {
            std::cout << "Pls enable bluetooth service on your device" << std::endl;
            return false;
        }
        std::vector<SimpleBLE::Adapter> adapters = SimpleBLE::Adapter::get_adapters();
        if (adapters.empty())
        {
            std::cout << "No bluetooth adapters found" << std::endl;
            return false;
        }
        setAdapter(adapters[0]);
        return true;
    }

    void setAdapter(const SimpleBLE::Adapter &adapt)
    {
        m_firstAdapter = adapt;
    }

    bool checkIfPeripheralIsPod(SimpleBLE::Peripheral &el)
    {
        return  el.rssi() >= MIN_RSSI and el.manufacturer_data().contains(AIRPODS_MANUFACTURER);
    }
};

#endif
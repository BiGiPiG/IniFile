#include "IniFile.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

//удаление лишних пробелов
std::string trim(std::string str) {
  std::string res;
  for (size_t i = 0; i < str.size(); ++i) {
    if (str[i] != ' ') {
      res.push_back(str[i]);
    }
  }
  return res;
}

//преобразование true value
std::string getValue(std::string str) {
  for (size_t i = 0; i < trueValues.size(); ++i) {
    if (trueValues[i] == str) {
      return "1";
    }
  }
  return str;
}

IniFile::IniFile(const std::string &path) : filePath(path) {
  
  std::ifstream file(path);
  if (!file) {
    std::cout << "error";
  }

  std::string cur_str; //текущая строка

  std::string cur_section; //текущая секция
  std::string key;         //текущий ключ
  std::string value;       //текущее значение ключа

  while (getline(file, cur_str)) {

    // удаление комментариев
    for (size_t i = 0; i < cur_str.size(); ++i) {
      if (cur_str[i] == ';') {
        cur_str = cur_str.substr(0, i);
      }
    }

    //проверка на секцию и получение имени секции
    if (std::count(cur_str.begin(), cur_str.end(), '[') == 1 &&
        std::count(cur_str.begin(), cur_str.end(), ']') == 1) {
      if (cur_str.find('[') < cur_str.find(']')) {
        size_t start, end;
        for (size_t i = 0; i < cur_str.size(); ++i) {
          if (cur_str[i] == '[') {
            start = i;
          }
          if (cur_str[i] == ']') {
            end = i;
          }
        }
        //обновление cur_section
        cur_section = cur_str.substr(start + 1, end - 1);
        if (!cur_section.empty()) {
          addNewSection(cur_section);
        }
      }
    }

    //проверка на строку типа "ключ-значение" и полуение ключа и значения
    if (std::count(cur_str.begin(), cur_str.end(), '=') == 1) {
      for (size_t i = 0; i < cur_str.size(); ++i) {
        if (cur_str[i] == '=') {
          key = trim(cur_str.substr(0, i));
          value = getValue(trim(cur_str.substr(i + 1, cur_str.size() - i)));
        }
      }

      //добавление ключа и значения
      if (!key.empty() && isSectionExist(cur_section) && !value.empty()) {
        writeString(cur_section, key, value);
      }
    }
  }
}

IniFile::~IniFile() { save(); }

void IniFile::save() {
  std::ofstream file(filePath);
  for (const auto &section : data) {
    file << "[" << section.first << "]" << std::endl;
    for (const auto &key : section.second) {
      file << key.first << "=" << key.second << std::endl;
    }
  }
}

int IniFile::readInt(const std::string &section, const std::string &key,
                     int def) {
  if (isSectionExist(section) && isKeysExist(section, key)) {
    return std::stoi(data[section][key]);
  }
  return def;
}

double IniFile::readDouble(const std::string &section, const std::string &key,
                           double def) {
  if (isSectionExist(section) && isKeysExist(section, key)) {
    return std::stod(data[section][key]);
  }
  return def;
}

std::string IniFile::readString(const std::string &section,
                                const std::string &key,
                                const std::string &def) {
  if (isSectionExist(section) && isKeysExist(section, key)) {
    return data[section][key];
  }
  return def;
}

bool IniFile::readBool(const std::string &section, const std::string &key,
                       bool def) {
  if (isSectionExist(section) && isKeysExist(section, key)) {
    if (data[section][key] == "1") {
      return true;
    }
  }
  return def;
}

void IniFile::writeInt(const std::string &section, const std::string &key,
                       int value) {
  data[section][key] = std::to_string(value);
}

void IniFile::writeDouble(const std::string &section, const std::string &key,
                          double value) {
    data[section][key] = std::to_string(value);
}

void IniFile::writeString(const std::string &section, const std::string &key,
                          const std::string &value) {
  data[section][key] = value;
}

void IniFile::writeBool(const std::string &section, const std::string &key,
                        bool value) {
  data[section][key] = getValue(std::to_string(value));
}

bool IniFile::isSectionExist(const std::string &section) {
  return data.find(section) != data.end();
}

bool IniFile::isKeysExist(const std::string &section, const std::string &key) {
  if (isSectionExist(section)) {
    return data[section].find(key) != data[section].end();
  }
  return false;
}

size_t IniFile::getSectionsCount() { return data.size(); }

size_t IniFile::getKeysCount(const std::string &section) {
  if (isSectionExist(section)) {
    return data[section].size();
  }
  return 0;
}

SectionsMap IniFile::getSections() const { return data; }

bool IniFile::deleteSection(const std::string &section) {
  if (isSectionExist(section)) {
    return data.erase(section) != 0;
  }
  return false;
}

bool IniFile::deleteKey(const std::string &section, const std::string &key) {
  if (isSectionExist(section)) {
    return data[section].erase(key) != 0;
  }
  return false;
}

bool IniFile::addNewSection(const std::string &section) {
  if (!isSectionExist(section)) {
    data[section] = KeysMap();
    return true;
  }
  return false;
}

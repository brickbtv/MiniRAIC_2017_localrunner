#ifndef CONFIG_H
#define CONFIG_H

#include <QJsonObject>

class Config
{
public:
    int elevators_per_player;
    int elevators_offset;
    int elevators_first_pos;
    int floors_first;
    int floors_count;
    int indicator_position;
    int ladder_position;
    int passenger_spawn_position;

    Config(QJsonObject& obj);
};

#endif // CONFIG_H

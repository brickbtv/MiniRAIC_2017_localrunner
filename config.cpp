#include "config.h"

Config::Config(QJsonObject& obj)
{
    elevators_per_player = obj["config"].toObject()["ELEVATORS_FOR_PASSENGER_COUNT"].toInt();
    elevators_offset = obj["config"].toObject()["ELEVATOR_IN_GROUP_OFFSET"].toInt();
    elevators_first_pos = obj["config"].toObject()["FIRST_ELEVATOR_POSITION"].toInt();
    floors_first = obj["config"].toObject()["FIRST_FLOOR"].toInt();
    floors_count = obj["config"].toObject()["FLOORS_COUNT"].toInt();
    indicator_position = obj["config"].toObject()["INDICATOR_POSITION"].toInt();
    ladder_position = obj["config"].toObject()["LADDER_POSITION"].toInt();
    passenger_spawn_position = obj["config"].toObject()["PASSENGER_SPAWN_POSITION"].toInt();
}

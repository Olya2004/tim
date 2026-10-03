#pragma once

#include <string>

#include "tim_translation.h"


namespace tim
{

/**
 * Одна запись справочной таблицы: как вызывается команда и что она делает.
 */
struct tcl_command_info
{
    std::string         usage;       ///< Синтаксис без имени, например "(dark|light)".
    tim::translations   description; ///< Краткое описание на поддерживаемых языках.
};

/**
 * Таблица «имя команды — описание» для /help.
 *
 * Список самих команд берётся динамически из a_script_engine::functions();
 * здесь хранятся только пояснения к ним. Заполняется макросом
 * TIM_TCL_REGISTER_HELP в момент регистрации команды.
 */
class tcl_help
{

public:

    static void add(const std::string &name,
                    const std::string &usage,
                    const tim::translations &description);

    /** \return Запись команды или nullptr, если описания нет. */
    static const tcl_command_info *find(const std::string &name);

};

}

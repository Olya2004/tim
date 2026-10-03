#include "tim_tcl_help.h"

#include <map>


// Static
static std::map<std::string, tim::tcl_command_info> &table()
{
    static std::map<std::string, tim::tcl_command_info> t;
    return t;
}


// Public
/**
 * Добавляет (или заменяет) описание команды. Повторная регистрация
 * в новой сессии просто перезаписывает ту же запись.
 *
 * \param name Имя команды без префикса '/'.
 * \param usage Синтаксис аргументов.
 * \param description Краткое описание.
 */
void tim::tcl_help::add(const std::string &name,
                        const std::string &usage,
                        const tim::translations &description)
{
    table()[name] = { usage, description };
}

/**
 * \param name Имя команды без префикса '/'.
 * \return Запись команды или nullptr, если описания нет.
 */
const tim::tcl_command_info *tim::tcl_help::find(const std::string &name)
{
    const auto it = table().find(name);
    return it == table().end() ? nullptr : &it->second;
}

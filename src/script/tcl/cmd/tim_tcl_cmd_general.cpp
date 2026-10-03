#include "tim_tcl_cmd_general.h"

#include "tim_a_terminal.h"
#include "tim_config.h"
#include "tim_tcl.h"
#include "tim_tcl_help.h"
#include "tim_tcl_cmd.h"
#include "tim_translator.h"

#include "lil.hpp"

#include <algorithm>
#include <cassert>
#include <string>
#include <unordered_set>
#include <vector>


// Static
/**
 * Команда `/quit`: закрывает ТЕКУЩУЮ сессию, не трогая остальной
 * сервер. Конкретное действие выполняет prompt_service, регистрирующий
 * обработчик через tcl->set_quit_handler.
 *
 * \param lil LIL-сессия.
 * \param argc Количество аргументов; команда не принимает ни одного.
 * \param argv Аргументы (не используются).
 * \return nullptr (команда не возвращает значения).
 */
static lil_value_t tim_tcl_cmd_quit(lil_t lil, size_t argc, lil_value_t *argv)
{
    (void) argv;

    if (argc)
    {
        lil_set_error(lil,
                      TIM_TR("No arguments expected."_en,
                             "Команда не имеет параметров."_ru));
        return nullptr;
    }

    tim::tcl *tcl = (tim::tcl *)lil_get_data(lil);
    assert(tcl);

    tcl->request_quit();

    return nullptr;
}

/**
 * Команда `/help [команда]`: без аргумента печатает все команды движка
 * (имена берутся из functions(), описания — из tcl_help), с аргументом —
 * синтаксис и описание одной команды. Цвета берутся из темы терминала.
 *
 * \param lil LIL-сессия.
 * \param argc 0 или 1.
 * \param argv argv[0] — имя команды (с '/' или без).
 * \return nullptr.
 */
static lil_value_t tim_tcl_cmd_help(lil_t lil, size_t argc, lil_value_t *argv)
{
    if (argc > 1)
    {
        lil_set_error(lil,
                      TIM_TR("Expected optional command name."_en,
                             "Ожидается необязательное имя команды."_ru));
        return nullptr;
    }

    tim::tcl *tcl = (tim::tcl *)lil_get_data(lil);
    assert(tcl);

    tim::a_terminal *term = tcl->terminal();
    const tim::terminal_color_theme &colors = term->theme().colors;
    const tim::color &c_head = colors.at(tim::terminal_color_index::info);
    const tim::color &c_cmd = colors.at(tim::terminal_color_index::em_text);
    const tim::color &c_text = colors.at(tim::terminal_color_index::text);

    const std::unordered_set<std::string> funcs = tcl->functions();

    if (argc == 1)
    {
        std::string name = lil_to_string(argv[0]);
        if (!name.empty()
                && name[0] == tim::COMMAND_PREFIX)
            name.erase(0, 1);

        if (!funcs.count(name))
        {
            lil_set_error(lil,
                          TIM_TR("Unknown command."_en,
                                 "Неизвестная команда."_ru));
            return nullptr;
        }

        const tim::tcl_command_info *info = tim::tcl_help::find(name);
        term->cprintf(c_cmd, tim::color(), "%c%s%s%s", tim::COMMAND_PREFIX,
                      name.c_str(),
                      info && !info->usage.empty() ? " " : "",
                      info ? info->usage.c_str() : "");
        term->reset_colors();
        term->printf("\n");
        term->cprintf(c_text, tim::color(), "  %s\n",
                      info
                          ? tim::translator::translate(info->description, __FILE__, __LINE__)
                          : TIM_TR("No description available."_en,
                                   "Описание отсутствует."_ru));
        term->reset_colors();
        return nullptr;
    }

    std::vector<std::string> described;
    std::vector<std::string> others;
    for (const std::string &f: funcs)
        (tim::tcl_help::find(f) ? described : others).push_back(f);
    std::sort(described.begin(), described.end());
    std::sort(others.begin(), others.end());

    term->cprintf(c_head, tim::color(), "%s\n",
                  TIM_TR("Commands:"_en, "Команды:"_ru));
    for (const std::string &name: described)
    {
        const tim::tcl_command_info *info = tim::tcl_help::find(name);
        term->cprintf(c_cmd, tim::color(), "  %c%s%s%s", tim::COMMAND_PREFIX,
                      name.c_str(),
                      info->usage.empty() ? "" : " ",
                      info->usage.c_str());
        term->cprintf(c_text, tim::color(), "\n      %s\n",
                      tim::translator::translate(info->description, __FILE__, __LINE__));
    }

    if (!others.empty())
    {
        term->cprintf(c_head, tim::color(), "%s\n",
                      TIM_TR("Other commands (no description):"_en,
                             "Остальные команды (без описания):"_ru));
        std::string line = " ";
        for (const std::string &name: others)
            line += " " + std::string(1, tim::COMMAND_PREFIX) + name;
        term->cprintf(c_text, tim::color(), "%s\n", line.c_str());
    }

    term->reset_colors();
    term->cprintf(c_head, tim::color(), "%s\n",
                  TIM_TR("Type /help <command> for details; Tab completes names."_en,
                         "Введите /help <команда> для подробностей; Tab дополняет имена."_ru));
    term->reset_colors();

    return nullptr;
}

// Public
/**
 * Регистрирует общие Tcl-команды (универсальные, не связанные с
 * конкретным сервисом): /quit, /clear и т.п. в указанной LIL-сессии.
 * Сейчас это только `quit`; другие общие команды добавлять сюда.
 *
 * \param lil LIL-сессия.
 */
void tim::tcl_add_general(lil_t lil)
{
    assert(lil);

    TIM_TCL_REGISTER_HELP(lil, quit, "",
                          "Close this session."_en,
                          "Закрыть текущую сессию."_ru);
    TIM_TCL_REGISTER_HELP(lil, help, "[command]",
                          "List commands, or describe one command."_en,
                          "Список команд или описание одной команды."_ru);
}

#pragma once

#include <cassert>

#include "tim_tcl_help.h"
#include "tim_translation.h"


/**
 * Сахар для регистрации Tcl-команды в LIL-сессии. При неудаче
 * lil_register возвращает <= 0 — попадание в эту ветку означает баг
 * (имя занято / пустая лямбда), такие случаи мы хотим увидеть в
 * debug-сборке через assert и не давать запуститься в release.
 *
 * \param lil LIL-сессия (lil_t).
 * \param name Идентификатор команды; нужна функция tim_tcl_cmd_<name>.
 */
#define TIM_TCL_REGISTER(lil, name) \
if (lil_register((lil), #name, tim_tcl_cmd_##name) <= 0) \
    return assert(0 && "Failed to register TCL command '" #name "'.");

/**
 * То же, что TIM_TCL_REGISTER, плюс запись в справочную таблицу /help.
 *
 * \param lil LIL-сессия (lil_t).
 * \param name Идентификатор команды.
 * \param usage Синтаксис аргументов (строка, "" если их нет).
 * \param ... Пары "описание"_en, "описание"_ru.
 */
#define TIM_TCL_REGISTER_HELP(lil, name, usage, ...) \
do { \
    TIM_TCL_REGISTER(lil, name); \
    tim::tcl_help::add(#name, (usage), { __VA_ARGS__ }); \
} while (0)

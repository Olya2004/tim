#include "tim_a_script_engine.h"

#include "tim_a_script_engine_p.h"

#include <algorithm>
#include <cassert>


// Public
/**
 * Конструктор. Сохраняет имя языка и указатель на терминал.
 *
 * Базовый класс абстрактный за счёт чисто виртуальных методов, поэтому
 * конструктор реально вызывается только из наследников.
 *
 * \param language Имя языка (например, "Tcl") — для логов и UI.
 * \param term Терминал, в который команды могут писать.
 */
tim::a_script_engine::a_script_engine(const std::string &language,
                                      tim::a_terminal *term)
    : _d()
{
    assert(!language.empty() && "Script language name must not be empty.");
    assert(term);

    _d->_language = language;
    _d->_terminal = term;
}

/** Виртуальный деструктор. PIMPL разрушает _d автоматически. */
tim::a_script_engine::~a_script_engine() = default;

/** \return Имя языка скрипт-движка. */
const std::string &tim::a_script_engine::language() const
{
    return _d->_language;
}

/** \return Терминал, к которому привязан движок. */
tim::a_terminal *tim::a_script_engine::terminal() const
{
    return _d->_terminal;
}

/**
 * Возвращает отсортированный список автодополнений для префикса:
 * ключевые слова и функции движка, начинающиеся с \a prefix. Те же
 * множества использует /help, так что список команд один на всё.
 *
 * \param prefix Префикс для поиска.
 * \return Подходящие имена по алфавиту.
 */
std::vector<std::string> tim::a_script_engine::complete(const std::string &prefix) const
{
    std::vector<std::string> res;

    for (const auto &set: { keywords(), functions() })
        for (const std::string &name: set)
            if (name.compare(0, prefix.size(), prefix) == 0)
                res.push_back(name);

    std::sort(res.begin(), res.end());
    res.erase(std::unique(res.begin(), res.end()), res.end());

    return res;
}

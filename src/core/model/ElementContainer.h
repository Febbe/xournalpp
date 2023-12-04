/*
 * Xournal++
 *
 * Interface for element containers like selection
 *
 * @author Xournal++ Team
 * https://github.com/xournalpp/xournalpp
 *
 * @license GNU GPLv2 or later
 */

#pragma once

#include <vector>

class Element;

class ElementContainer {
public:
    virtual auto getElements() const -> std::vector<Element*> = 0;
    virtual ~ElementContainer() = default;
};

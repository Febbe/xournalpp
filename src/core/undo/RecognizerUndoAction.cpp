#include "RecognizerUndoAction.h"

#include <memory>  // for __shared_ptr_access, __shared_ptr_acces...

#include <glib.h>  // for g_warning

#include "model/Element.h"
#include "model/Layer.h"      // for Layer
#include "model/Stroke.h"     // for Stroke
#include "model/XojPage.h"    // for XojPage
#include "undo/UndoAction.h"  // for UndoAction
#include "util/Stacktrace.h"  // for Stacktrace
#include "util/i18n.h"        // for _

class Control;

RecognizerUndoAction::RecognizerUndoAction(const PageRef& page, Layer* layer, Stroke* original, Stroke* recognized):
        UndoAction("RecognizerUndoAction"), layer(layer), recognized(recognized) {
    this->page = page;

    addSourceElement(original);
}

RecognizerUndoAction::~RecognizerUndoAction() = default;

void RecognizerUndoAction::addSourceElement(Stroke* s) {
    for (Stroke* s2: this->original) {
        if (s2 == s) {
            g_warning("RecognizerUndoAction::addSourceElement() twice the same\n");
            Stacktrace::printStracktrace();
            return;
        }
    }

    this->original.push_back(s);
}

auto RecognizerUndoAction::undo(Control* control) -> bool {
    auto [owned, pos] = this->layer->removeElement(this->recognized);
    this->recognizedOwned = std::move(owned);

    this->page->fireElementChanged(this->recognized);

    for (auto&& s: this->originalOwned) {
        auto sptr = s.get();
        this->layer->insertElement(std::move(s), pos);
        this->page->fireElementChanged(sptr);
    }
    this->originalOwned.clear();

    this->undone = true;
    return true;
}

auto RecognizerUndoAction::redo(Control* control) -> bool {
    Element::Index pos = 0;
    for (Stroke* s: this->original) {
        auto [owned, posi] = this->layer->removeElement(s);
        pos = posi;
        this->page->fireElementChanged(s);
        this->originalOwned.push_back(std::move(owned));
    }
    this->layer->insertElement(std::move(this->recognizedOwned), pos);

    this->page->fireElementChanged(this->recognized);

    this->undone = false;
    return true;
}

auto RecognizerUndoAction::getText() -> std::string { return _("Stroke recognizer"); }

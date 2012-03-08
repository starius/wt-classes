/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#include <boost/foreach.hpp>

#include <Wt/WApplication>
#include <Wt/WFormWidget>
#include <Wt/WLabel>
#include <Wt/WString>
#include <Wt/WTableCell>
#include <Wt/WTableColumn>
#include <Wt/WTableRow>
#include <Wt/WText>
#include <Wt/WWidget>

#include "TableForm.hpp"
#include "util.hpp"

namespace Wt {

namespace Wc {

const int TF_SECTION_COLUMN = 0;
const int TF_NAME_COLUMN = 0;
const int TF_INPUT_COLUMN = 1;
const int TF_DESCRIPTION_COLUMN = 2;
const int TF_COLUMN_SPAN = 3;
const int TF_COMMENT_COLUMN = TF_INPUT_COLUMN;
const int TF_COMMENT_COLUMN_SPAN = 2;

TableForm::TableForm(WContainerWidget* parent):
    WTable(parent) {
    setStyleClass("wt_tableform");
    wApp->useStyleSheet(config_value("resourcesURL", "resources/") +
                        "Wc/css/table_form.css");
}

void TableForm::section(const WString& header) {
    WTableCell* cell = elementAt(rowCount(), TF_SECTION_COLUMN);
    cell->setColumnSpan(TF_COLUMN_SPAN);
    new WText(header, cell);
    cell->setStyleClass("wt_tableform_header");
}

WContainerWidget* TableForm::item(const WString& name,
                                  const WString& description, WFormWidget* fw,
                                  WWidget* input, bool row) {
    int row_num = rowAt(rowCount())->rowNum();
    WTableCell* name_cell = elementAt(row_num, TF_NAME_COLUMN);
    WTableCell* input_cell = elementAt(row_num, TF_INPUT_COLUMN);
    WTableCell* description_cell = elementAt(row_num, TF_DESCRIPTION_COLUMN);
    if (!row) {
        input_cell = name_cell;
        description_cell = name_cell;
        name_cell->setColumnSpan(TF_COLUMN_SPAN);
    }
    if (row) {
        name_cell->setStyleClass("wt_tableform_name");
        input_cell->setStyleClass("wt_tableform_input");
        description_cell->setStyleClass("wt_tableform_description");
    }
    WLabel* name_label = new WLabel(name, name_cell);
    name_label->setInline(false);
    if (!description.empty()) {
        WText* description_text = new WText(description, description_cell);
        description_text->setInline(false);
    }
    if (fw) {
        name_label->setBuddy(fw);
    }
    if (input) {
        inputs_.push_back(input);
        input_cell->addWidget(input);
        comment_cell(input)->setColumnSpan(TF_COMMENT_COLUMN_SPAN);
        comment_cell(input)->setStyleClass("wt_tableform_comment");
        comment_cell(input)->hide();
    }
    return input_cell;
}

void TableForm::show(WWidget* input) {
    parent_row_(input)->show();
}

void TableForm::hide(WWidget* input) {
    parent_row_(input)->hide();
}

void TableForm::foreach(const boost::function<void(WWidget*)>& f) {
    BOOST_FOREACH (WWidget* input, inputs_) {
        f(input);
    }
}

void TableForm::set_comment(WWidget* input, const WString& message) {
    comment_cell(input)->clear();
    if (message.empty()) {
        comment_cell(input)->hide();
    } else {
        comment_cell(input)->addWidget(new WText(message));
        comment_cell(input)->show();
    }
}

WTableRow* TableForm::parent_row_(WWidget* input) {
    return rowAt(DOWNCAST<WTableCell*>(input->parent())->row());
}

WTableCell* TableForm::comment_cell(WWidget* input) {
    return elementAt(parent_row_(input)->rowNum() + 1, TF_COMMENT_COLUMN);
}

}

}


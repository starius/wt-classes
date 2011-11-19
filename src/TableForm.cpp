/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * wt-classes is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#include <Wt/WFormWidget>
#include <Wt/WLabel>
#include <Wt/WString>
#include <Wt/WTableCell>
#include <Wt/WTableColumn>
#include <Wt/WTableRow>
#include <Wt/WText>
#include <Wt/WWidget>

#include "TableForm.hpp"

namespace Wt {

const int TF_SECTION_COLUMN = 0;
const int TF_NAME_COLUMN = 0;
const int TF_INPUT_COLUMN = 1;
const int TF_DESCRIPTION_COLUMN = 2;
const int TF_NOROW_COLUMN = 2;
const int TF_COLUMN_SPAN = 3;
const int TF_WIDGET_IN_ROW = 0;
const int TF_WIDGET_IN_NOROW = 2;
const int TF_WIDGETS_IN_NOROW = 3;

TableForm::TableForm(Wt::WContainerWidget* parent):
    Wt::WTable(parent) {
    setStyleClass("wt_tableform");
}

void TableForm::section(const Wt::WString& header) {
    Wt::WTableCell* cell = elementAt(rowCount(), TF_SECTION_COLUMN);
    cell->setColumnSpan(TF_COLUMN_SPAN);
    new Wt::WText(header, cell);
    cell->setStyleClass("wt_tableform_header");
}

Wt::WContainerWidget* TableForm::item(const Wt::WString& name,
                                      const Wt::WString& description, Wt::WFormWidget* fw,
                                      Wt::WWidget* input, bool row) {
    int row_num = rowAt(rowCount())->rowNum();
    Wt::WTableCell* name_cell = elementAt(row_num, TF_NAME_COLUMN);
    Wt::WTableCell* input_cell = elementAt(row_num, TF_INPUT_COLUMN);
    Wt::WTableCell* description_cell = elementAt(row_num, TF_DESCRIPTION_COLUMN);
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
    Wt::WLabel* name_label = new Wt::WLabel(name, name_cell);
    name_label->setInline(false);
    if (!description.empty()) {
        Wt::WText* description_text = new Wt::WText(description, description_cell);
        description_text->setInline(false);
    }
    if (fw) {
        name_label->setBuddy(fw);
    }
    if (input) {
        input_cell->addWidget(input);
    }
    return input_cell;
}

void TableForm::show(Wt::WWidget* input) {
    parent_row_(input)->show();
}

void TableForm::hide(Wt::WWidget* input) {
    parent_row_(input)->hide();
}

void TableForm::foreach(const boost::function<void(WWidget*)>& f) {
    for (int row = 0; row < rowCount(); row++) {
        WWidget* input = input_at(row);
        if (input) {
            f(input);
        }
    }
}

Wt::WTableRow* TableForm::parent_row_(Wt::WWidget* input) {
    return rowAt(dynamic_cast<Wt::WTableCell*>(input->parent())->row());
}

WWidget* TableForm::input_at(int row) {
    int column_span = elementAt(row, TF_NAME_COLUMN)->columnSpan();
    if (column_span == 1) {
        WTableCell* cell = elementAt(row, TF_INPUT_COLUMN);
        return cell->widget(TF_WIDGET_IN_ROW);
    } else if (column_span == TF_COLUMN_SPAN) {
        WTableCell* cell = elementAt(row, TF_NOROW_COLUMN);
        if (cell->count() == TF_WIDGETS_IN_NOROW) {
            return cell->widget(TF_WIDGET_IN_NOROW);
        }
    }
    return 0;
}

}


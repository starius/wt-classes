/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

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
const int TF_NOROW_COLUMN = 2;
const int TF_COLUMN_SPAN = 3;
const int TF_WIDGET_IN_ROW = 0;
const int TF_WIDGET_IN_NOROW = 2;
const int TF_WIDGETS_IN_NOROW = 3;

TableForm::TableForm(WContainerWidget* parent):
    WTable(parent) {
    setStyleClass("wt_tableform");
    std::string resourcesURL = "resources/";
    wApp->readConfigurationProperty("resourcesURL", resourcesURL);
    wApp->useStyleSheet(resourcesURL + "Wc/css/table_form.css");
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
        input_cell->addWidget(input);
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
    for (int row = 0; row < rowCount(); row++) {
        WWidget* input = input_at(row);
        if (input) {
            f(input);
        }
    }
}

WTableRow* TableForm::parent_row_(WWidget* input) {
    return rowAt(downcast<WTableCell*>(input->parent())->row());
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

}


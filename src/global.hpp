/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WC_GLOBAL_HPP_
#define WC_GLOBAL_HPP_

namespace Wt {

/** Namespace for wt-classes library */
namespace Wc {

class ConstrainedSpinBox;
class ConstrainedDoubleSpinBox;
class IntervalWidget;
class LogicalPaintedWidget;
class TableForm;
class TimeDurationWidget;
struct Row3;
struct ThreeWPoints;
struct Matrix3x3;

class AbstractArgument;
class AbstractInput;
class FormWidgetInput;
class LineEditInput;
class FileInput;
class TextFileInput;
class BoolInput;
class AbstractOutput;
class FileOutput;
class ViewFileOutput;
class AbstractTask;
class TableTask;
class AbstractRunner;
class ForkingRunner;
class AbstractQueue;
class TaskNumberQueue;

namespace url {

class Parser;
class Node;
class PredefinedNode;
class IntegerNode;
class StringNode;

}

namespace notify {

class Event;
class Widget;
class Server;
class Task;
class PlanningServer;

}

}

}

#endif


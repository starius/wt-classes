// This may look like C code, but it's really -*- C++ -*-
/*
 * Copyright (C) 2008 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */

#include "FileTreeTable.hpp"
#include "FileTreeTableNode.hpp"

#include <Wt/WText>

namespace Wt {
namespace Wc {


    FileTreeTable::FileTreeTable(const boost::filesystem::path& path, 
                                 const std::string &suffix,
			     WContainerWidget *parent)
  : WTreeTable(parent)
{
  addColumn("Size", 80);
  addColumn("Modified", 110);

  header(1)->setStyleClass("fsize");
  header(2)->setStyleClass("date");

  setTreeRoot(new FileTreeTableNode(path, suffix), "File");

  treeRoot()->setImagePack("icons/");
  treeRoot()->expand();
}
}
}

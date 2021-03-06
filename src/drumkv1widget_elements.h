// drumkv1widget_elements.h
//
/****************************************************************************
   Copyright (C) 2012-2015, rncbc aka Rui Nuno Capela. All rights reserved.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*****************************************************************************/

#ifndef __drumkv1widget_elements_h
#define __drumkv1widget_elements_h

#include <QTreeView>


// Forwards.
class drumkv1widget_elements_model;

class drumkv1_ui;

class drumkv1_sample;

class QDragEnterEvent;
class QDragMoveEvent;
class QDropEvent;


//----------------------------------------------------------------------------
// drumkv1widget_elements -- Custom (tree) list view.

class drumkv1widget_elements : public QTreeView
{
	Q_OBJECT

public:

	// Constructor.
	drumkv1widget_elements(QWidget *pParent = 0);

	// Destructor.
	~drumkv1widget_elements();

	// Settlers.
	void setInstance(drumkv1_ui *pDrumkUi);
	drumkv1_ui *instance() const;

	// Current element accessors.
	void setCurrentIndex(int row);
	int currentIndex() const;

	// Refreshener.
	void refresh();

signals:

	// Emitted signals.
	void itemActivated(int);
	void itemDoubleClicked(int);

	// Load new sample file on current item.
	void itemLoadSampleFile(const QString&, int);

protected slots:

	// Internal slot handlers.
	void currentRowChanged(const QModelIndex&, const QModelIndex&);
	void doubleClicked(const QModelIndex&);

protected:

	// Mouse interaction.
	void mousePressEvent(QMouseEvent *pMouseEvent);
	void mouseMoveEvent(QMouseEvent *pMouseEvent);
	void mouseReleaseEvent(QMouseEvent *pMouseEvent);

	// Drag-n-drop (more of the later) support.
	void dragEnterEvent(QDragEnterEvent *pDragEnterEvent);
	void dragMoveEvent(QDragMoveEvent *pDragMoveEvent);
	void dropEvent(QDropEvent *pDropEvent);

	// Reset drag/select state.
	void resetDragState();

	// Default size hint.
	QSize sizeHint() const;

private:

	// Instance variables.
	drumkv1widget_elements_model *m_pModel;

	// Drag state.
	enum DragState { DragNone = 0, DragStart } m_dragState;

	QPoint m_posDrag;

	drumkv1_sample *m_pDragSample;
};


#endif  // __drumkv1widget_elements_h

// end of drumkv1widget_elements.h

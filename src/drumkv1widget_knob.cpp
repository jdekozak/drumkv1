// drumkv1widget_knob.cpp
//
/****************************************************************************
   Copyright (C) 2012, rncbc aka Rui Nuno Capela. All rights reserved.

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

#include "drumkv1widget_knob.h"

#include <QLabel>
#include <QDial>
#include <QSpinBox>
#include <QComboBox>

#include <QGridLayout>

#include <QMouseEvent>

#include <math.h>


// Integer value round.
inline int iroundf(float x) { return int(x < 0.0f ? x - 0.5f : x + 0.5f); }


//-------------------------------------------------------------------------
// drumkv1widget_knob - Custom composite widget.
//

// Constructor.
drumkv1widget_knob::drumkv1widget_knob ( QWidget *pParent ) : QWidget(pParent)
{
	const QFont& font = QWidget::font();
	const QFont font3(font.family(), font.pointSize() - 3);
	QWidget::setFont(font3);

	m_pLabel = new QLabel();
	m_pDial  = new QDial();

	resetDefaultValue();

	m_pLabel->setAlignment(Qt::AlignCenter);
	m_pDial->setSingleStep(10);
	m_pDial->setNotchesVisible(true);
	m_pDial->setMaximumSize(QSize(48, 42));

	QGridLayout *pGridLayout = new QGridLayout();
	pGridLayout->setMargin(0);
	pGridLayout->setSpacing(0);
	pGridLayout->addWidget(m_pLabel, 0, 0, 1, 3);
	pGridLayout->addWidget(m_pDial,  1, 0, 1, 3);
	QWidget::setLayout(pGridLayout);

	QWidget::setMaximumSize(QSize(48, 72));

	QObject::connect(m_pDial,
		SIGNAL(valueChanged(int)),
		SLOT(dialValueChanged(int)));
}


void drumkv1widget_knob::setText ( const QString& sText )
{
	m_pLabel->setText(sText);
}

QString drumkv1widget_knob::text (void) const
{
	return m_pLabel->text();
}


void drumkv1widget_knob::setValue ( float fValue )
{
	bool bDialBlock = m_pDial->blockSignals(true);

	m_pDial->setValue(iroundf(100.0f * fValue));

	QPalette pal;
	if (m_iDefaultValue < 1) {
		m_fDefaultValue = fValue;
		m_iDefaultValue++;
	}
	else
	if (QWidget::isEnabled()
		&& ::fabs(fValue - m_fDefaultValue) > 0.001f) {
		pal.setColor(QPalette::Base,
			(pal.window().color().value() < 0x7f
				? QColor(Qt::darkYellow).darker()
				: QColor(Qt::yellow).lighter()));
	}
	QWidget::setPalette(pal);

	emit valueChanged(value());

	m_pDial->blockSignals(bDialBlock);
}

float drumkv1widget_knob::value (void) const
{
	return float(m_pDial->value()) / 100.0f;
}


void drumkv1widget_knob::setMaximum ( float fMaximum )
{
	m_pDial->setMaximum(iroundf(100.0f * fMaximum));
}

float drumkv1widget_knob::maximum (void) const
{
	return float(m_pDial->maximum()) / 100.0f;
}


void drumkv1widget_knob::setMinimum ( float fMinimum )
{
	m_pDial->setMinimum(iroundf(100.0f * fMinimum));
}

float drumkv1widget_knob::minimum (void) const
{
	return float(m_pDial->minimum()) / 100.0f;
}


void drumkv1widget_knob::resetDefaultValue (void)
{
	m_fDefaultValue = 0.0f;
	m_iDefaultValue = 0;
}

void drumkv1widget_knob::setDefaultValue ( float fDefaultValue )
{
	m_fDefaultValue = fDefaultValue;
	m_iDefaultValue++;
}

float drumkv1widget_knob::defaultValue (void) const
{
	return m_fDefaultValue;
}


void drumkv1widget_knob::setSingleStep ( float fSingleStep )
{
	m_pDial->setSingleStep(iroundf(100.0f * fSingleStep));
}

float drumkv1widget_knob::singleStep (void) const
{
	return float(m_pDial->singleStep()) / 100.0f;
}


// Mouse behavior event handler.
void drumkv1widget_knob::mousePressEvent ( QMouseEvent *pMouseEvent )
{
	if (pMouseEvent->button() == Qt::MidButton) {
		if (m_iDefaultValue < 1) {
			m_fDefaultValue = 0.5f * (maximum() + minimum());
			m_iDefaultValue++;
		}
		setValue(m_fDefaultValue);
	}

	QWidget::mousePressEvent(pMouseEvent);
}


// Internal widget slots.
void drumkv1widget_knob::dialValueChanged ( int iDialValue )
{
	setValue(float(iDialValue) / 100.0f);
}


//-------------------------------------------------------------------------
// drumkv1widget_spin - Custom knob/spin-box widget.
//

// Constructor.
drumkv1widget_spin::drumkv1widget_spin ( QWidget *pParent )
	: drumkv1widget_knob(pParent)
{
	m_pSpinBox = new QSpinBox();
	m_pSpinBox->setAccelerated(true);
	m_pSpinBox->setAlignment(Qt::AlignCenter);

	const QFontMetrics fm(drumkv1widget_knob::font());
	m_pSpinBox->setMaximumHeight(fm.lineSpacing() + (fm.height() >> 1));

	QGridLayout *pGridLayout
		= static_cast<QGridLayout *> (QWidget::layout());
	pGridLayout->addWidget(m_pSpinBox, 2, 1, 1, 1);

	setMinimum(0.0f);
	setMaximum(1.0f);

	QObject::connect(m_pSpinBox,
		SIGNAL(valueChanged(int)),
		SLOT(spinBoxValueChanged(int)));
}


void drumkv1widget_spin::setValue ( float fValue )
{
	bool bSpinBlock = m_pSpinBox->blockSignals(true);

	m_pSpinBox->setValue(iroundf(100.0f * fValue));
	drumkv1widget_knob::setValue(fValue);

	m_pSpinBox->blockSignals(bSpinBlock);
}


void drumkv1widget_spin::setMaximum ( float fMaximum )
{
	m_pSpinBox->setMaximum(iroundf(100.0f * fMaximum));
	drumkv1widget_knob::setMaximum(fMaximum);
}


void drumkv1widget_spin::setMinimum ( float fMinimum )
{
	m_pSpinBox->setMinimum(iroundf(100.0f * fMinimum));
	drumkv1widget_knob::setMinimum(fMinimum);
}


// Internal widget slots.
void drumkv1widget_spin::spinBoxValueChanged ( int iSpinValue )
{
	setValue(float(iSpinValue) / 100.0f);
}


// Special value text (minimum)
void drumkv1widget_spin::setSpecialValueText ( const QString& sText )
{
	m_pSpinBox->setSpecialValueText(sText);
}

QString drumkv1widget_spin::specialValueText (void) const
{
	return m_pSpinBox->specialValueText();
}


//-------------------------------------------------------------------------
// drumkv1widget_combo - Custom knob/combo-box widget.
//

// Constructor.
drumkv1widget_combo::drumkv1widget_combo ( QWidget *pParent )
	: drumkv1widget_knob(pParent)
{
	m_pComboBox = new QComboBox();

	const QFontMetrics fm(drumkv1widget_knob::font());
	m_pComboBox->setMaximumHeight(fm.lineSpacing() + (fm.height() >> 1));

	setSingleStep(1);

	QGridLayout *pGridLayout
		= static_cast<QGridLayout *> (QWidget::layout());
	pGridLayout->addWidget(m_pComboBox, 2, 0, 1, 3);

	QObject::connect(m_pComboBox,
		SIGNAL(activated(int)),
		SLOT(comboBoxValueChanged(int)));
}


void drumkv1widget_combo::setValue ( float fValue )
{
	bool bComboBlock = m_pComboBox->blockSignals(true);

	int iValue = iroundf(fValue);
	m_pComboBox->setCurrentIndex(iValue);
	drumkv1widget_knob::setValue(float(iValue));

	m_pComboBox->blockSignals(bComboBlock);
}



// Special combo-box mode accessors.
void drumkv1widget_combo::insertItems ( int iIndex, const QStringList& items )
{
	m_pComboBox->insertItems(iIndex, items);

	setMinimum(0.0f);
	setMaximum(float(m_pComboBox->count() - 1));
}

void drumkv1widget_combo::clear (void)
{
	m_pComboBox->clear();

	setMinimum(0.0f);
	setMaximum(1.0f);
}


// Internal widget slots.
void drumkv1widget_combo::comboBoxValueChanged ( int iComboValue )
{
	setValue(float(iComboValue));
}


// Reimplemented mouse-wheel stepping.
void drumkv1widget_combo::wheelEvent ( QWheelEvent *pWheelEvent )
{
	int delta = (pWheelEvent->delta() / 120);
	if (delta) {
		float fValue = value() + float(delta);
		if (fValue < minimum())
			fValue = minimum();
		else
		if (fValue > maximum())
			fValue = maximum();
		setValue(fValue);
	}
}


// end of drumkv1widget_knob.cpp

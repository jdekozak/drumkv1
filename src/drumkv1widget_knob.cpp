// drumkv1widget_knob.cpp
//
/****************************************************************************
   Copyright (C) 2012-2014, rncbc aka Rui Nuno Capela. All rights reserved.

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
	const QFont font2(font.family(), font.pointSize() - 2);
	QWidget::setFont(font2);

	m_pLabel = new QLabel();
	m_pDial  = new QDial();

	m_fScale = 100.0f;

	m_fDefaultValue = 0.0f;
	m_iDefaultValue = 0;

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

	QWidget::setMaximumSize(QSize(52, 72));

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

	m_pDial->setValue(scaleFromValue(fValue));

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
	return valueFromScale(m_pDial->value());
}


QString drumkv1widget_knob::valueText (void) const
{
	return QString::number(m_pDial->value());
}


void drumkv1widget_knob::setMaximum ( float fMaximum )
{
	m_pDial->setMaximum(scaleFromValue(fMaximum));
}

float drumkv1widget_knob::maximum (void) const
{
	return valueFromScale(m_pDial->maximum());
}


void drumkv1widget_knob::setMinimum ( float fMinimum )
{
	m_pDial->setMinimum(scaleFromValue(fMinimum));
}

float drumkv1widget_knob::minimum (void) const
{
	return valueFromScale(m_pDial->minimum());
}


void drumkv1widget_knob::resetDefaultValue (void)
{
	if (m_iDefaultValue < 1) {
		m_fDefaultValue = 0.5f * (maximum() + minimum());
		m_iDefaultValue++;
	}

	setValue(m_fDefaultValue);
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
	m_pDial->setSingleStep(scaleFromValue(fSingleStep));
}

float drumkv1widget_knob::singleStep (void) const
{
	return valueFromScale(m_pDial->singleStep());
}


// Mouse behavior event handler.
void drumkv1widget_knob::mousePressEvent ( QMouseEvent *pMouseEvent )
{
	if (pMouseEvent->button() == Qt::MidButton)
		resetDefaultValue();

	QWidget::mousePressEvent(pMouseEvent);
}


// Internal widget slots.
void drumkv1widget_knob::dialValueChanged ( int iDialValue )
{
	setValue(valueFromScale(iDialValue));
}



// Scale multiplier (default=100).
void drumkv1widget_knob::setScale ( float fScale )
{
	m_fScale = fScale;
}

float drumkv1widget_knob::scale (void) const
{
	return m_fScale;
}


// Scale/value converters.
int drumkv1widget_knob::scaleFromValue ( float fValue ) const
{
	return iroundf(m_fScale * fValue);
}

float drumkv1widget_knob::valueFromScale ( int iScale ) const
{
	return float(iScale) / m_fScale;
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
	m_pSpinBox->setMaximumHeight(fm.height() + 6);

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

	m_pSpinBox->setValue(scaleFromValue(fValue));
	drumkv1widget_knob::setValue(fValue);

	m_pSpinBox->blockSignals(bSpinBlock);
}


void drumkv1widget_spin::setMaximum ( float fMaximum )
{
	m_pSpinBox->setMaximum(scaleFromValue(fMaximum));
	drumkv1widget_knob::setMaximum(fMaximum);
}


void drumkv1widget_spin::setMinimum ( float fMinimum )
{
	m_pSpinBox->setMinimum(scaleFromValue(fMinimum));
	drumkv1widget_knob::setMinimum(fMinimum);
}


// Internal widget slots.
void drumkv1widget_spin::spinBoxValueChanged ( int iSpinValue )
{
	setValue(valueFromScale(iSpinValue));
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
	m_pComboBox->setMaximumHeight(fm.height() + 6);

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


QString drumkv1widget_combo::valueText (void) const
{
	return m_pComboBox->currentText();
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

/*
    SPDX-FileCopyrightText: 2010 Pierre Marchand <pierre@oep-h.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "familywidget.h"
#include "ui_familywidget.h"
#include "typotek.h"
#include "fmpreviewlist.h"
#include "fmfontdb.h"
#include "fontitem.h"
#include "fminfodisplay.h"
#include "floatingwidgetsregister.h"
#include "samplewidget.h"
#include "chartwidget.h"
#include "activationwidget.h"
#include "fmvariants.h"


#include <QColor>
#include <QListWidgetItem>
#include <QIcon>
#include <QDebug>


FamilyWidget::FamilyWidget(QWidget *parent) :
		QWidget(parent),
		ui(new Ui::FamilyWidget),
		sample(nullptr),
		chart(nullptr),
		activation(nullptr),
		currentIndex(0),
		currentPage(FAMILY_VIEW_INFO)
{
	ui->setupUi(this);


	ui->familyPreview->setNumCol(1);
	ui->familyPreview->setModelColumn(1);
	ui->familyPreview->setViewMode(QListView::IconMode);
	ui->familyPreview->setIconSize(QSize(ui->familyPreview->width(), 1.3 * typotek::getInstance()->getPreviewSize() * typotek::getInstance()->getDpiY() / 72.0));
	ui->familyPreview->setUniformItemSizes(true);
	ui->familyPreview->setMovement(QListView::Static);

	previewModel = new FMPreviewModel( this, ui->familyPreview );
	previewModel->setSpecString(QStringLiteral("<variant>"));
	ui->familyPreview->setModel(previewModel);



	connect(ui->returnListButton, &QPushButton::clicked, this, &FamilyWidget::backToList);
	connect(ui->familyPreview, &FMPreviewView::widthChanged, this, &FamilyWidget::slotPreviewUpdateSize);
	connect(ui->familyPreview, &FMPreviewView::activated, this, &FamilyWidget::slotPreviewSelected);
	connect(ui->familyPreview, &FMPreviewView::clicked, this, &FamilyWidget::slotPreviewSelected);
	connect(ui->familyPreview, &FMPreviewView::pressed, this, &FamilyWidget::slotPreviewSelected);
	connect(ui->infoButton, &QToolButton::clicked, this, &FamilyWidget::slotShowInfo);
	connect(ui->sampleButton, &QToolButton::clicked, this, &FamilyWidget::slotShowSample);
	connect(ui->chartButton, &QToolButton::clicked, this, &FamilyWidget::slotShowChart);
	connect(ui->activationButton, &QToolButton::clicked, this, &FamilyWidget::slotShowActivation);

}

FamilyWidget::~FamilyWidget()
{
	delete ui;
}

TagsWidget* FamilyWidget::tagWidget()
{
	return ui->tagsWidget;
}

void FamilyWidget::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void FamilyWidget::keyPressEvent(QKeyEvent *event)
{
	if(event->modifiers().testFlag(Qt::ControlModifier) && (event->key() == Qt::Key_PageUp))
	{
		switch(currentPage)
		{
		case FAMILY_VIEW_SAMPLE: slotShowActivation();
			break;
		case FAMILY_VIEW_CHART: slotShowSample();
			break;
		case FAMILY_VIEW_INFO: slotShowChart();
			break;
		case FAMILY_VIEW_ACTIVATION: slotShowInfo();
		default:break;
		}
	}
	else if(event->modifiers().testFlag(Qt::ControlModifier) && (event->key() == Qt::Key_PageDown))
	{
		switch(currentPage)
		{
		case FAMILY_VIEW_INFO: slotShowActivation();
			break;
		case FAMILY_VIEW_ACTIVATION: slotShowSample();
			break;
		case FAMILY_VIEW_SAMPLE: slotShowChart();
			break;
		case FAMILY_VIEW_CHART: slotShowInfo();
		default:break;
		}
	}
}

void FamilyWidget::slotPreviewUpdateSize(int w)
{
	ui->familyPreview->setIconSize(QSize(w, 1.3 * typotek::getInstance()->getPreviewSize() * typotek::getInstance()->getDpiY() / 72.0));
}

void FamilyWidget::setFamily(const QString &f)
{
	if(f != family)
	{
		family = f;
		ui->familyLabel->setText(family);
		QList<FontItem*> fl( FMVariants::Order(FMFontDb::DB()->FamilySet(family)));
		ui->tagsWidget->prepare(fl);
		previewModel->resetBase(fl);
		if(!fl.isEmpty())
		{
			ui->familyPreview->setCurrentIndex( previewModel->index(0) );
			curVariant = fl.first()->path();
			Q_EMIT fontSelected(curVariant);
		}
		delete sample;
		sample = nullptr;
		delete chart;
		chart = nullptr;
		delete activation;
		activation = nullptr;

		uniBlock = QString();
		slotShowSample();
	}
}

void FamilyWidget::slotPreviewUpdate()
{
	previewModel->dataChanged();
}

void FamilyWidget::slotPreviewSelected(const QModelIndex &index)
{
	QString fid(index.data(FMPreviewModel::PathRole).toString());
	if(fid != curVariant)
	{
		if(chart != nullptr)
			uniBlock = reinterpret_cast<ChartWidget*>(chart)->currentBlock();
		delete sample;
		delete chart;
		sample = chart = nullptr;
		curVariant = fid;
		currentIndex = index.row();
		switch(currentPage)
		{
		case FAMILY_VIEW_INFO: slotShowInfo();
			break;
		case FAMILY_VIEW_SAMPLE: slotShowSample();
			break;
		case FAMILY_VIEW_CHART: slotShowChart();
			break;
		default:
			break;
		}

		Q_EMIT fontSelected(curVariant);
	}
}


void FamilyWidget::slotShowSample()
{
	FloatingWidget * fw(FloatingWidgetsRegister::Widget(curVariant, SampleWidget::Name));
	if(fw == nullptr)
	{
		if(nullptr == sample)
		{
			auto sw(new SampleWidget(curVariant, ui->pageSample));
			ui->displayStack->insertWidget(FAMILY_VIEW_SAMPLE, sw);
			sample = sw;
			connect(sample, &FloatingWidget::detached, this, &FamilyWidget::slotDetachSample);
		}
		ui->displayStack->setCurrentWidget(sample);
	}
	else
	{
		fw->show();
	}
	currentPage = FAMILY_VIEW_SAMPLE;
	updateButtons();
}

void FamilyWidget::slotShowInfo()
{
	FMInfoDisplay fid(FMFontDb::DB()->Font(curVariant));
	ui->webView->setHtml(fid.getHtml());
	ui->displayStack->setCurrentIndex(FAMILY_VIEW_INFO);
	currentPage = FAMILY_VIEW_INFO;
	updateButtons();
}

void FamilyWidget::slotShowChart()
{
	FloatingWidget * fw(FloatingWidgetsRegister::Widget(curVariant, ChartWidget::Name));
	if(fw == nullptr)
	{
		if(nullptr == chart)
		{
			auto cw(new ChartWidget(curVariant, uniBlock, ui->pageChart));
			ui->displayStack->insertWidget(FAMILY_VIEW_CHART, cw);
			chart = cw;
			connect(chart, &FloatingWidget::detached, this, &FamilyWidget::slotDetachChart);
		}
		ui->displayStack->setCurrentWidget(chart);
	}
	else
	{
		fw->show();
	}
	currentPage = FAMILY_VIEW_CHART;
	updateButtons();
}

void FamilyWidget::slotShowActivation()
{
	FloatingWidget * fw(FloatingWidgetsRegister::Widget(curVariant, ActivationWidget::Name));
	if(fw == nullptr)
	{
		if(nullptr == activation)
		{
			auto aw(new ActivationWidget(family, ui->pageActivation));
			ui->displayStack->insertWidget(FAMILY_VIEW_ACTIVATION, aw);
			activation = aw;
			connect(aw, &ActivationWidget::familyStateChanged, this, &FamilyWidget::slotStateChange);
		}
		ui->displayStack->setCurrentWidget(activation);
	}
	else
	{
		fw->show();
	}
	currentPage = FAMILY_VIEW_ACTIVATION;
	updateButtons();
}

void FamilyWidget::slotDetachSample()
{
	disconnect(sample, &FloatingWidget::detached, this, &FamilyWidget::slotDetachSample);
	sample = nullptr;
	slotShowInfo();
}

void FamilyWidget::slotDetachChart()
{
	disconnect(chart, &FloatingWidget::detached, this, &FamilyWidget::slotDetachChart);
	uniBlock = reinterpret_cast<ChartWidget*>(chart)->currentBlock();
	chart = nullptr;
	slotShowInfo();
}

void FamilyWidget::slotStateChange()
{
	previewModel->resetBase(FMVariants::Order(FMFontDb::DB()->FamilySet(family)));
	Q_EMIT familyStateChanged();
}

void FamilyWidget::updateButtons()
{
	static QList<QToolButton*> buttons;
	if(buttons.isEmpty())
	{
		buttons << ui->sampleButton
				<< ui->infoButton
				<< ui->chartButton
				<< ui->activationButton;
		for (auto* b : std::as_const(buttons))
		{
			b->setCheckable(true);
		}
	}
	for (auto* b : std::as_const(buttons))
	{
		b->setChecked(false);
	}
	switch(currentPage)
	{
	case FAMILY_VIEW_SAMPLE: ui->sampleButton->setChecked(true);
		break;
	case FAMILY_VIEW_ACTIVATION: ui->activationButton->setChecked(true);
		break;
	case FAMILY_VIEW_CHART: ui->chartButton->setChecked(true);
		break;
	case FAMILY_VIEW_INFO : ui->infoButton->setChecked(true);
		break;
	default:break;
	}
}

#include "moc_familywidget.cpp"

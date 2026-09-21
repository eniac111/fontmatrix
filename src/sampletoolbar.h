/*
    SPDX-FileCopyrightText: 2010 Pierre Marchand <pierre@oep-h.com>

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef SAMPLETOOLBAR_H
#define SAMPLETOOLBAR_H

#include <QWidget>
#include <QStringList>

namespace Ui {
    class SampleToolBar;
}

class SampleToolBar : public QWidget
{
    Q_OBJECT

public:
    explicit SampleToolBar(QWidget *parent = nullptr);
    ~SampleToolBar() override;

    enum Button{
	    SampleButton,
	    OpenTypeButton
    };

    [[nodiscard]] double getFontSize() const;
    void setFontSize(double fs);

    bool isChecked(Button b);
    void toggle(Button b, bool c);

    void enableButton(Button b, bool c);

    void setScripts(const QStringList& ll);
    QString getScript();

protected:
    void changeEvent(QEvent *e) override;

private:
    Ui::SampleToolBar *const ui;

Q_SIGNALS:
    void SizeChanged(double);
    void SampleToggled(bool);
    void OpenTypeToggled(bool);
    void ScriptSelected();
};

#endif // SAMPLETOOLBAR_H

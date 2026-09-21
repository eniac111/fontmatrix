#ifndef OPENCLOSEARROW_H
#define OPENCLOSEARROW_H

#include <QLabel>


class OpenCloseArrow : public QLabel
{
	Q_OBJECT

	QString baseText;

public:
	explicit OpenCloseArrow(QWidget *parent = nullptr, bool open = true);
	~OpenCloseArrow() override;

	bool isOpen() const {return openingState;}

private:
	bool openingState;
	void setOpText();

protected:
	void mouseReleaseEvent( QMouseEvent * ev ) override;

Q_SIGNALS:
	void openChanged(bool);

public Q_SLOTS:
	void changeOpen(bool t);

};

#endif // OPENCLOSEARROW_H

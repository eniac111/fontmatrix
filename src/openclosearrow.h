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

signals:
	void openChanged(bool);

public slots:
	void changeOpen(bool t);

};

#endif // OPENCLOSEARROW_H

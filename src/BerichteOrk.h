#ifndef BERICHTEORK_H
#define BERICHTEORK_H

#include <QMainWindow>

#include "ui_mainwindow.h"
#include "DataHandler.h"

class QSqlRecord;

class BerichteOrk : public QMainWindow, public Ui::BerichteOrk
{
	Q_OBJECT

public:
	BerichteOrk(QWidget *parent = 0);

private:
	void initializeModels();
	void initializeViews();
	void recalculateWeeks();
	void disableAllElements(bool toggle);
	DataHandler dataHandler;
	QSqlTableModel* schuleModel;
	QSqlTableModel* betriebModel;

	int selectedYear;
	int selectedWeek;

private slots:
	void on_actionAnimateYears_toggled(bool toggled);
	void on_wochenTree_itemSelectionChanged();
	void on_removeBetriebButton_clicked();
	void on_removeSchuleButton_clicked();
	void on_addBetriebButton_clicked();
	void on_addSchuleButton_clicked();
	void betriebModel_beforeInsert(QSqlRecord &record);
	void schuleModel_beforeInsert(QSqlRecord &record);
	void on_actionOpen_triggered();

};

#endif
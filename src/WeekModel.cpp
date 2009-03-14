#include "WeekModel.h"

WeekModel::WeekModel(QDate &startDate, QDate &endDate, QObject *parent /* = 0 */)
	: QAbstractItemModel(parent)
{
	rootItem = new WeekModelItem();
	setDateRange(startDate, endDate);
}

WeekModel::~WeekModel()
{
	delete rootItem;
}

QModelIndex WeekModel::index(int row, int column, const QModelIndex &parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	WeekModelItem *parentItem;

	if (!parent.isValid())
		parentItem = rootItem;
	else
		parentItem = static_cast<WeekModelItem*>(parent.internalPointer());

	WeekModelItem *childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();
}

QModelIndex WeekModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	WeekModelItem *childItem = static_cast<WeekModelItem*>(index.internalPointer());
	WeekModelItem *parentItem = childItem->parent();

	if (parentItem == rootItem)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

int WeekModel::rowCount(const QModelIndex &parent) const
{
	WeekModelItem *parentItem;

	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = rootItem;
	else
		parentItem = static_cast<WeekModelItem*>(parent.internalPointer());

	return parentItem->childCount();
}
 
int WeekModel::columnCount(const QModelIndex &parent) const
{
	if (parent.isValid())
		return static_cast<WeekModelItem*>(parent.internalPointer())->columnCount();
	else
		return rootItem->columnCount();
}

QVariant WeekModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role != Qt::DisplayRole &&
		role != Qt::EditRole)
		return QVariant();

	WeekModelItem *item = static_cast<WeekModelItem*>(index.internalPointer());

	if (role == Qt::DisplayRole)
	{
		// Wenn der Vater invalid ist, ist das item ein �bergeordnetes Objekt
		// -> Jahr
		if (!index.parent().isValid())
			return QString("%1").arg(item->year());
		// Wenn das item einen Vater hat ist es eine Kalenderwoche
		else
			return QString(tr("KW %1")).arg(item->week());
	}
	else
		return item->data(index.column());
}

Qt::ItemFlags WeekModel::flags(const QModelIndex &index) const
{
	if (!index.isValid())
		return 0;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant WeekModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
		return rootItem->data(section);

	return QVariant();
}

void WeekModel::setDateRange(QDate &startDate, QDate &endDate)
{
	// Jahre und Kalenderwochen aus dem Datum holen
	int startYear, endYear, currFirstWeek, currLastWeek;
	int startWeek =	startDate.weekNumber(&startYear);
	int endWeek = endDate.weekNumber(&endYear);

	for (int i = startYear; i <= endYear; i++)
	{
		if (i == startYear)
			currFirstWeek = startWeek;
		else
			currFirstWeek = 1;

		if (i == endYear)
			currLastWeek = endWeek;
		else
		{
			// Wenn das aktuelle Jahr mit einem Donnerstag anf�ngt
			// oder endet -> 53 KWs. Sonst 52 KWs.
			if (QDate(i, 1, 1).dayOfWeek() == Qt::Thursday ||
				QDate(i, 12, 31).dayOfWeek() == Qt::Thursday)
				currLastWeek = 53;
			else
				currLastWeek = 52;
		}

		// WeekModelItem f�r das Jahr erstellen
		WeekModelItem *newYearItem = new WeekModelItem(i, 0, rootItem);
		rootItem->appendChild(newYearItem);

		// Einzelne Wochen zur Liste hinzuf�gen
		for (int j = currFirstWeek; j <= currLastWeek; j++)
		{
			WeekModelItem *newWeekItem = new WeekModelItem(i, j, newYearItem);
			newYearItem->appendChild(newWeekItem);
			weekMap.insert(((i*100) + j), newWeekItem);
		}
	}
}

QModelIndex WeekModel::week(int year, int week) const
{
	// Map-Index aus den Parametern bauen
	int index = (year * 100) + week;

	// Existiert ein Element mit diesem Index?
	if (weekMap.contains(index))
	{
		// QModelIndex erstellen und zur�ckgeben
		return createIndex(weekMap.value(index)->row(), 0, weekMap.value(index));
	}
	else
		// So ein Element gibt es nicht
		return QModelIndex();
}
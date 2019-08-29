/*
#
# Cyan Tile Item
#
# Copyright Ole-André Rodlie, FxArena DA.
#
# ole.andre.rodlie@gmail.com / support@fxarena.net
#
# This software is governed either by the CeCILL or the CeCILL-C license
# under French law and abiding by the rules of distribution of free software.
# You can  use, modify and or redistribute the software under the terms of
# the CeCILL or CeCILL-C licenses as circulated by CEA, CNRS and INRIA
# at the following URL: "http://www.cecill.info".
#
# As a counterpart to the access to the source code and  rights to copy,
# modify and redistribute granted by the license, users are provided only
# with a limited warranty  and the software's author,  the holder of the
# economic rights,  and the successive licensors  have only  limited
# liability.
#
# In this respect, the user's attention is drawn to the risks associated
# with loading,  using,  modifying and/or developing or reproducing the
# software by the user in light of its specific status of free software,
# that may mean  that it is complicated to manipulate,  and  that  also
# therefore means  that it is reserved for developers  and  experienced
# professionals having in-depth computer knowledge. Users are therefore
# encouraged to load and test the software's suitability as regards their
# requirements in conditions enabling the security of their systems and/or
# data to be ensured and,  more generally, to use and operate it in the
# same conditions as regards security.
#
# The fact that you are presently reading this means that you have had
# knowledge of the CeCILL and CeCILL-C licenses and that you accept its terms.
#
*/

#include "CyanTileItem.h"

#include <QPointF>
#include <QPen>

CyanTileItem::CyanTileItem(QGraphicsItem *parent, QGraphicsPixmapItem *pixmapItem)
    : QGraphicsRectItem(parent)
    , _pixmap(pixmapItem)
{
    QPen newPen(Qt::transparent);
    newPen.setWidth(0);
    setPen(newPen);
}

void CyanTileItem::setPixmapItem(QGraphicsPixmapItem *pixmapItem)
{
    if (!pixmapItem) { return; }
    _pixmap = pixmapItem;
}

QGraphicsPixmapItem *CyanTileItem::getPixmapItem()
{
    if (!_pixmap) { return new QGraphicsPixmapItem(); }
    else { return _pixmap; }
}

void CyanTileItem::setPixmap(const QPixmap &pixmap)
{
    if (!_pixmap) { return; }
    _pixmap->setPixmap(pixmap);
}

void CyanTileItem::setPixmap(int id, const QPixmap &pixmap)
{
    if (!_pixmap || this->data(0).toInt() != id) { return; }
    setPixmap(pixmap);
}

#pragma once

/*
 * Copyright (c) 2009, Thomas Jaeger <ThJaeger@gmail.com>
 * Copyright (c) 2023, Daniel Kondor <kondor.dani@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "motion.h"

namespace libgestures
{

struct Point {
    qreal x;
    qreal y;
    qreal t;
    qreal alpha;
};

class Stroke
{
public:
    Stroke() = default;
    Stroke(const std::vector<QPointF> &pointsRaw);
    Stroke(const std::vector<Point> &points);

    const std::vector<Point> &points() const;

    qreal compare(const Stroke &other) const;
    static double min_matching_score();

private:
    void finish();

    void step(const Stroke &other,
              const int &N,
              std::vector<qreal> &dist,
              std::vector<int> &prevX,
              std::vector<int> &prevY,
              const int &x,
              const int &y,
              const qreal &tx,
              const qreal &ty,
              int *k,
              const int &x2,
              const int &y2) const;

    /**
     * Converts the specified list of deltas to a path that starts at (0,0).
     */
    static std::vector<QPointF> deltasToPath(const std::vector<QPointF> &deltas);

    /**
     * Simplifies the specified path using the Ramer-Douglas–Peucker algorithm.
     */
    static std::vector<QPointF> simplify(const std::vector<QPointF> &points, const qreal &epsilon = 10);

    static qreal perpendicularDistance(const QPointF &point, const QPointF &lineStart, const QPointF &lineEnd);
    static void ramerDouglasPeucker(const std::vector<QPointF> &points, qreal epsilon, std::vector<QPointF> &out);

    std::vector<Point> m_points;
};

/**
 * A motion input action where a shape is drawn.
 */
class StrokeTrigger : public MotionTrigger
{
public:
    const std::vector<Stroke> &strokes() const;
    void setStrokes(const std::vector<Stroke> &strokes);

private:
    std::vector<Stroke> m_strokes;
};

}
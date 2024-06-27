
/* Copyright (C) 2019 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#pragma once

#include <QDialog>
#include <vector>

namespace Ui {
class ListDialog;
} // namespace Ui

class QListWidgetItem;

namespace cao {
class ListDialog final : public QDialog
{
    Q_OBJECT

public:
    enum class Sorting : std::uint8_t
    {
        Text,
        Insertion
    };

    explicit ListDialog(Sorting sort_by, QWidget *parent = nullptr);

    ListDialog(const ListDialog &) = delete;
    ListDialog(ListDialog &&)      = delete;

    auto operator=(const ListDialog &) -> ListDialog & = delete;
    auto operator=(ListDialog &&) -> ListDialog      & = delete;

    ~ListDialog() override;

    void add_item(QListWidgetItem *item);

    [[nodiscard]] auto get_choices() -> std::vector<const QListWidgetItem *>;
    [[nodiscard]] auto items() -> std::vector<QListWidgetItem *>;

    void set_user_add_item_visible(bool visible);

    void set_checked_items(const QString &text, bool add_missing_items = true);
    void set_checked_items(const QStringList &text_list, bool add_missing_items = true);

private:
    void add_user_item();
    void filter_view(const QString &text);
    [[nodiscard]] auto find_insert_pos(const QListWidgetItem *item) -> int;

    std::unique_ptr<Ui::ListDialog> ui_;
    Sorting sorting_ = Sorting::Insertion;
};
} // namespace cao

#!/usr/bin/env bash

set -euo pipefail

ROOT="/Users/sacha/Documents"

MAILMAP=$(mktemp)

cat > "$MAILMAP" <<'EOF'
Pinkupansa <sacha.cerf@polytechnique.org> <sacha@dhcp202.dmi.ens.fr>
Pinkupansa <sacha.cerf@polytechnique.org> <sacha@mac.home>
Pinkupansa <sacha.cerf@polytechnique.org> <sacha@MacBook-Pro-de-Sacha.local>
EOF

find "$ROOT" -type d -name ".git" 2>/dev/null | while read -r gitdir; do
    repo="$(dirname "$gitdir")"

    if git -C "$repo" log --all --format='%ae' 2>/dev/null | \
       grep -E '^(sacha@dhcp202\.dmi\.ens\.fr|sacha@mac\.home|sacha@MacBook-Pro-de-Sacha\.local)$' \
       >/dev/null; then

        echo
        echo "================================================="
        echo "Rewriting: $repo"
        echo "================================================="

        (
            cd "$repo"

            git filter-repo \
                --mailmap "$MAILMAP" \
                --force

            # Push all branches
            if git remote get-url origin >/dev/null 2>&1; then
                echo "Force-pushing branches..."
                git push origin --force --all

                echo "Force-pushing tags..."
                git push origin --force --tags
            else
                echo "No origin remote found, skipping push."
            fi
        )
    fi
done

rm -f "$MAILMAP"

echo
echo "Done."
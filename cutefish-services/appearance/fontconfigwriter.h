
#include <string>
#include <vector>
n#pragma once

struct FontConfig
{
    QString family;
    QString fixedFamily;
    bool antialias = true;
    QString hintStyle = "hintfull";
    QString rgba = "rgb";
    // Last resort for Han, kana and hangul the families above lack.
    QString cjkFamily = "Noto Sans CJK SC";
};

// Writes ~/.config/fontconfig/conf.d/99-lingmo.conf, falling back to installed families.
void writeFontConfig(const FontConfig &config);

// The UI font a language, such as zh_Hans_CN or zh_CN, starts with; empty
// when it has none of its own.
QString fontForLocale(const QString &code);

// The first Chinese, Japanese or Korean language decides the forms of Han text
// that carries no language of its own.
QString cjkFontForLanguages(const QStringList &languages);

#endif

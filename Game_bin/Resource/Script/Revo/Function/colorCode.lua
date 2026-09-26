-- By ham999dy
-- RevoTeam
-- 2018


colorCodeAr = {
	['Red'] = '<#FF0000>',
	['Blue'] = '<#0000FF>',
	['Yellow'] = '<#FFFF00>',
	['Green'] = '<#008000>',
	['Purple'] = '<#800080>',
	['Orange'] = '<#FFA500>',
	['Silver'] = '<#C0C0C0>',
	['Magenta'] = '<#FF00FF>',
	['RedWine'] = '<#990012>',
};


function colorCode(str, color)
	return colorCodeAr[color]..get_string(str);
end
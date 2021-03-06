
strs = [
	"The %A% %N% is looking very %A% this time of year.",
	"After reviewing the %N%, it is safe to say that the %A% %N% is more %A% than usual.",
	"We are confident that our team can be both %A% and %A%.",
	"Although we didn't hit our projections regarding the %N%, we think that this is because our %N% is very %A%.",
	"If I could describe our %N% in three words, it would be these: %A%, %A%, and %A%.",
	"We exceeded our %A% goals for the %N%, but only because we cut back on the most %A% of our %N%.",
	"I'm constantly approached on the street by people asking about how %A% our %N% is. To be frank--it is very %A%.",
	"When I wake up in the morning, my mother looks me dead in my eyes. She asks me, 'Are you going to make your %N% more %A% this year?' Every morning, I tell her the same thing: my %N% is as %A% as the rising sun."
]
adjs = [
	"fiscal",
	"budgetary",
	"economic",
	"financial",
	"commercial",
	"lucrative",
	"monetary"
]
nouns = [
	"profit",
	"revenue",
	"quarter",
	"wholesale",
	"expense",
	"business",
	"spreadsheet",
	"report",
	"data",
	"income",
]

import random
for i in range(20):
	sentence = random.choice(strs)
	while "%A%" in sentence:
		sentence = sentence.replace("%A%", random.choice(adjs), 1)
	while "%N%" in sentence:
		sentence = sentence.replace("%N%", random.choice(nouns), 1)
	print(sentence, end=" ")

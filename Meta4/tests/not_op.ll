declare i32 @atoi(i8* nocapture) nounwind readonly
@.stritoa = private unnamed_addr constant [3 x i8] c"%d\00"
define i8* @itoa(i32 %n, i8* %buf) {
	%1 = alloca i32
	%2 = alloca i8*
	store i32 %n, i32* %1
	store i8* %buf, i8** %2
	%3 = load i8** %2
	%4 = load i32* %1
	%5 = call i32 (i8*, i8*, ...)* @sprintf(i8* %3, i8* getelementptr inbounds ([3 x i8]* @.stritoa, i32 0, i32 0), i32 %4)
	%6 = load i8** %2
	ret i8* %6
}
declare i32 @sprintf(i8*, i8*, ...)
declare i32 @puts(i8* nocapture) nounwind
define i32 @main () {
	%a = alloca i32
	%b = alloca i32
	%c = alloca i8
	%buf = alloca [20 x i8]
	store i32 0, i32* %a
	store i32 1, i32* %b
	%1 = trunc i32 1 to i8
	store i8 %1, i8* %c
	br label %lbl1
	lbl1:
	br label %lbl4
	lbl4:
	%2 = load i32* %a
	%3 = icmp ne i32 %2, 0
	br i1 %3 , label %lbl5 , label %lbl6
	lbl5:
	%4 = load i32* %b
	%5 = icmp ne i32 %4, 0
	br label %lbl6
	lbl6:
	%6 = phi i1 [ false, %lbl4 ], [ %5, %lbl5 ]
	%7 = zext i1 %6 to i32
	%8 = icmp ne i32 %7, 0
	br i1 %8 , label %lbl2 , label %lbl3
	lbl2:
	%9 = load i8* %c
	%10 = sext i8 %9 to i32
	%11 = icmp ne i32 %10, 0
	br label %lbl3
	lbl3:
	%12 = phi i1 [ false, %lbl6 ], [ %11, %lbl2 ]
	%13 = zext i1 %12 to i32
	ret i32 0
}

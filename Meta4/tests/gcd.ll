@.str = private unnamed_addr constant [32 x i8] c"\45\72\72\6f\72\3a\20\74\77\6f\20\70\61\72\61\6d\65\74\65\72\73\20\72\65\71\75\69\72\65\64\2e\00"
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
@buffer = common global [20 x i8] zeroinitializer
define i32 @gcd (i32 %a, i32 %b) {
	%1 = alloca i32
	store i32 %a, i32* %1
	%2 = alloca i32
	store i32 %b, i32* %2
	%3 = load i32* %1
%4 = icmp eq i32 %3, 0
%5 = zext i1 %4 to i32
	%6 = trunc i32 %5 to i1	br i1 %6, label %lbl1 , label %lbl2
	lbl1:
	%7 = load i32* %2
	ret i32 %7
	br label %lbl3
	lbl2:
	br label %lbl4
	lbl4:
	%9 = load i32* %2
%10 = icmp sgt i32 %9, 0
%11 = zext i1 %10 to i32
	%12 = trunc i32 %11 to i1	br i1 %12, label %lbl5 , label %lbl6
	lbl5:
	%13 = load i32* %1
	%14 = load i32* %2
%15 = icmp sgt i32 %13, %14
%16 = zext i1 %15 to i32
	%17 = trunc i32 %16 to i1	br i1 %17, label %lbl7 , label %lbl8
	lbl7:
	%18 = load i32* %1
	%19 = load i32* %2
	%20 = sub i32 %18, %19
	store i32 %20, i32* %1
	br label %lbl9
	lbl8:
	%21 = load i32* %2
	%22 = load i32* %1
	%23 = sub i32 %21, %22
	store i32 %23, i32* %2
	br label %lbl9
	lbl9:
	br label %lbl4
	lbl6:
	%24 = load i32* %1
	ret i32 %24
	br label %lbl3
	lbl3:
	ret i32 0
}
define i32 @main (i32 %argc, i8** %argv) {
	%1 = alloca i32
	store i32 %argc, i32* %1
	%2 = alloca i8**
	store i8** %argv, i8*** %2
	%a = alloca i32
	%b = alloca i32
	%3 = load i32* %1
%4 = icmp sgt i32 %3, 2
%5 = zext i1 %4 to i32
	%6 = trunc i32 %5 to i1	br i1 %6, label %lbl10 , label %lbl11
	lbl10:
	%7 = load i8*** %2
	%8 = getelementptr inbounds i8** %7, i32 1
	%9 = load i8** %8
	%10 = call i32 @atoi(i8* %9)
	store i32 %10, i32* %a
	%11 = load i8*** %2
	%12 = getelementptr inbounds i8** %11, i32 2
	%13 = load i8** %12
	%14 = call i32 @atoi(i8* %13)
	store i32 %14, i32* %b
	%15 = load i32* %a
	%16 = load i32* %b
	%17 = call i32 @gcd(i32 %15, i32 %16)
	%18 = getelementptr inbounds [20 x i8]* @buffer, i32 0, i32 0
	%19 = call i8* @itoa(i32 %17, i8* %18)
	%20 = call i32 @puts(i8* %19)
	br label %lbl12
	lbl11:
	%21 = getelementptr inbounds [32 x i8]* @.str, i32 0, i32 0
	%22 = call i32 @puts(i8* %21)
	br label %lbl12
	lbl12:
	ret i32 0
	ret i32 0
}

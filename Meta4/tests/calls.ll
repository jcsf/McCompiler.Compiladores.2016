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
define void @f (i32 %i) {
	%1 = alloca i32
	store i32 %i, i32* %1
	%2 = load i32* %1
%3 = icmp slt i32 %2, 10
%4 = zext i1 %3 to i32
	%5 = trunc i32 %4 to i1	br i1 %5, label %lbl1 , label %lbl2
	lbl1:
	%6 = load i32* %1
	%7 = add i32 %6, 1
	call void @f(i32 %7)
	br label %lbl3
	lbl2:
	br label %lbl3
	lbl3:
	ret void
}
define i32 @main (i32 %argc, i8** %argv) {
	%1 = alloca i32
	store i32 %argc, i32* %1
	%2 = alloca i8**
	store i8** %argv, i8*** %2
	%b = alloca i32*
	call void @f(i32 0)
	ret i32 0
}

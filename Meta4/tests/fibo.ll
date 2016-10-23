; ModuleID = 'fibo.c'
target datalayout = "e-m:e-p:32:32-f64:32:64-f80:32-n8:16:32-S128"
target triple = "i386-pc-linux-gnu"

@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1

; Function Attrs: nounwind
define i8* @itoa(i32 %n, i8* %buf) #0 {
  %1 = alloca i32, align 4
  %2 = alloca i8*, align 4
  store i32 %n, i32* %1, align 4
  store i8* %buf, i8** %2, align 4
  %3 = load i8** %2, align 4
  %4 = load i32* %1, align 4
  %5 = call i32 (i8*, i8*, ...)* @sprintf(i8* %3, i8* getelementptr inbounds ([3 x i8]* @.str, i32 0, i32 0), i32 %4) #3
  %6 = load i8** %2, align 4
  ret i8* %6
}

; Function Attrs: nounwind
declare i32 @sprintf(i8*, i8*, ...) #0

; Function Attrs: nounwind
define i32 @main(i32 %argc, i8** %argv) #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i8**, align 4
  %buf = alloca [20 x i8], align 1
  %n = alloca i32, align 4
  %i = alloca i32, align 4
  %c = alloca i32, align 4
  store i32 0, i32* %1
  store i32 %argc, i32* %2, align 4
  store i8** %argv, i8*** %3, align 4
  %4 = load i8*** %3, align 4
  %5 = getelementptr inbounds i8** %4, i32 1
  %6 = load i8** %5, align 4
  %7 = call i32 @atoi(i8* %6) #4
  store i32 %7, i32* %n, align 4
  store i32 0, i32* %i, align 4
  store i32 1, i32* %c, align 4
  br label %8

; <label>:8                                       ; preds = %20, %0
  %9 = load i32* %c, align 4
  %10 = load i32* %n, align 4
  %11 = icmp sle i32 %9, %10
  br i1 %11, label %12, label %23

; <label>:12                                      ; preds = %8
  %13 = load i32* %i, align 4
  %14 = call i32 @Fibonacci(i32 %13)
  %15 = getelementptr inbounds [20 x i8]* %buf, i32 0, i32 0
  %16 = call i8* @itoa(i32 %14, i8* %15)
  %17 = call i32 @puts(i8* %16)
  %18 = load i32* %i, align 4
  %19 = add nsw i32 %18, 1
  store i32 %19, i32* %i, align 4
  br label %20

; <label>:20                                      ; preds = %12
  %21 = load i32* %c, align 4
  %22 = add nsw i32 %21, 1
  store i32 %22, i32* %c, align 4
  br label %8

; <label>:23                                      ; preds = %8
  ret i32 0
}

; Function Attrs: nounwind readonly
declare i32 @atoi(i8*) #1

declare i32 @puts(i8*) #2

; Function Attrs: nounwind
define i32 @Fibonacci(i32 %n) #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 %n, i32* %2, align 4
  %3 = load i32* %2, align 4
  %4 = icmp eq i32 %3, 0
  br i1 %4, label %5, label %6

; <label>:5                                       ; preds = %0
  store i32 0, i32* %1
  br label %18

; <label>:6                                       ; preds = %0
  %7 = load i32* %2, align 4
  %8 = icmp eq i32 %7, 1
  br i1 %8, label %9, label %10

; <label>:9                                       ; preds = %6
  store i32 1, i32* %1
  br label %18

; <label>:10                                      ; preds = %6
  %11 = load i32* %2, align 4
  %12 = sub nsw i32 %11, 1
  %13 = call i32 @Fibonacci(i32 %12)
  %14 = load i32* %2, align 4
  %15 = sub nsw i32 %14, 2
  %16 = call i32 @Fibonacci(i32 %15)
  %17 = add nsw i32 %13, %16
  store i32 %17, i32* %1
  br label %18

; <label>:18                                      ; preds = %10, %9, %5
  %19 = load i32* %1
  ret i32 %19
}

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { nounwind readonly "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #3 = { nounwind }
attributes #4 = { nounwind readonly }

!llvm.ident = !{!0}

!0 = metadata !{metadata !"Ubuntu clang version 3.5.0-4ubuntu2~trusty2 (tags/RELEASE_350/final) (based on LLVM 3.5.0)"}

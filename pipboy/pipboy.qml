import QtQuick 1.1

Rectangle {
    id: mainScreen
    width: 500
    height: 319

    BorderImage {
        id: border_image1
        x: 0
        y: 0
        width: 500
        height: 319
        source: "pipboy_backdrop_rawFrame copy.png"
    }

    Text {
        id: text1
        x: 44
        y: 14
        text: qsTr("ACCEL")
        opacity: 0
        font.pixelSize: 12
    }

    TextInput {
        id: text_input1
        x: 119
        y: 95
        width: 80
        height: 20
        text: qsTr("Text")
        opacity: 0
        font.pixelSize: 12
    }

    Text {
        id: text2
        x: 44
        y: 101
        text: qsTr("Text")
        opacity: 0
        font.pixelSize: 12
    }

    Text {
        id: text3
        x: 44
        y: 142
        text: qsTr("Text")
        opacity: 0
        font.pixelSize: 12
    }

    Text {
        id: text4
        x: 44
        y: 202
        text: qsTr("Text")
        opacity: 0
        font.pixelSize: 12
    }

    TextInput {
        id: text_input2
        x: 129
        y: 150
        width: 80
        height: 20
        text: qsTr("Text")
        opacity: 0
        font.pixelSize: 12
    }

    TextInput {
        id: text_input3
        x: 129
        y: 202
        width: 80
        height: 20
        text: qsTr("Text")
        opacity: 0
        font.pixelSize: 12
    }

    property bool branch         //Controls if a state has a branch
    property int currentIndex:0  //Current index for outer loop
    property int innerCounter:0  //Current index for inner (branch) loop
    //statesParameters property holds two parameters for each state: a bool value if
    //a state has or has no branch and the number of states in a branch
    property variant statesParameters:[[true,5],[false,1],[true,5],[false,1],[true,5]]
    //Images could be stored in a list
    property variant imagesList: [["kiparisi/kip1.jpg","kiparisi/piramidalen.jpg",
                                   "kiparisi/spiral.jpg","kiparisi/tuya.jpg","kiparisi/septe.jpg"]]
    //A rectangle that contains explanatory text is added
//    Rectangle {
//        x:180;y:180
//        Image {id:picture; source:"Qt_logo.jpg"}
//    }

    property variant stateNames:[["state11","state12","state13","state14","state15"],
        ["state2"],
        ["state31","state32","state33","state34","state35"],
        ["state4"],
        ["state51","state52","state53","state54","state55"]
    ]

// Timer code carried over from sample code to iterate through states
//    property int counter:0
//    Timer {id:zen
//        interval: 2000; running: true; repeat: true
//        onTriggered: {
//            if(counter<5)
//                branch=statesParameters[counter][0];
//            else
//                Qt.quit();
//            if(branch==false)
//            {
//                innerCounter=0;
//                top1.state=stateNames[counter][innerCounter];
//                counter=counter+1;
//                currentIndex=counter;
//            }
//            else
//            {
//                if(innerCounter<statesParameters[counter][1])
//                {
//                    top1.state=stateNames[counter][innerCounter];
//                    innerCounter=innerCounter+1;
//                }
//                else{
//                    counter=currentIndex+1;
//                    if(counter>=5) Qt.quit();
//                }
//            }
//        }//end of onTriggered
//    }// end of Timer

    states:[
        State {
            name:"state11"
            PropertyChanges {target:top1;color:"Snow"}
            StateChangeScript {
                name:"stateScript11"
                script:{
                    literal.text="Cupressaceae - State 11"
                    picture.source=imagesList[0][2]
                }
            }

            PropertyChanges {
                target: text1
                x: 44
                y: 12
                width: 77
                height: 22
                color: "#8ff1ab"
                text: "ACCEL"
                font.bold: true
                horizontalAlignment: "AlignHCenter"
                font.pointSize: 14
                opacity: 1
            }

            PropertyChanges {
                target: list_view1
                x: 64
                y: 75
                cacheBuffer: 0
                spacing: 1
                orientation: "Vertical"
                boundsBehavior: "StopAtBounds"
                flickableDirection: "VerticalFlick"
                opacity: 1
            }

            PropertyChanges {
                target: row1
                x: 2
                y: 90
                width: 82
                height: 20
                spacing: 10
                clip: false
            }

            PropertyChanges {
                target: text_input1
                x: 127
                y: 95
                width: 71
                height: 20
                color: "#2ff78f"
                font.family: "Arial"
                horizontalAlignment: "AlignHCenter"
                font.pointSize: 11
                opacity: 1
            }

            PropertyChanges {
                target: text2
                x: 62
                y: 94
                width: 10
                height: 21
                color: "#2ff78f"
                text: "X:"
                font.family: "Arial"
                font.bold: true
                font.pointSize: 11
                horizontalAlignment: "AlignLeft"
                verticalAlignment: "AlignVCenter"
                opacity: 1
            }

            PropertyChanges {
                target: text3
                x: 62
                y: 150
                color: "#2ff78f"
                text: "Y:"
                font.family: "Arial"
                font.bold: true
                verticalAlignment: "AlignVCenter"
                font.pointSize: 11
                opacity: 1
            }

            PropertyChanges {
                target: text4
                x: 62
                y: 202
                color: "#2ff78f"
                text: "Z:"
                font.bold: true
                font.family: "Arial"
                font.pointSize: 11
                verticalAlignment: "AlignVCenter"
                opacity: 1
            }

            PropertyChanges {
                target: text_input2
                x: 127
                y: 150
                width: 72
                height: 20
                color: "#2ff78f"
                horizontalAlignment: "AlignHCenter"
                font.family: "Arial"
                font.pointSize: 11
                opacity: 1
            }

            PropertyChanges {
                target: text_input3
                x: 127
                y: 202
                width: 73
                height: 20
                color: "#2ff78f"
                horizontalAlignment: "AlignHCenter"
                font.family: "Arial"
                font.pointSize: 11
                opacity: 1
            }
        },
        State {
            name:"state12"
            PropertyChanges {target:top1;color:"Azure"}
            StateChangeScript {
                name:"stateScript12"
                script:{
                    literal.text="Cupressaceae - State 12"
                    picture.source=imagesList[0][0]
                }
            }

        },
        State {
            name:"state13"
            PropertyChanges {target:top1;color:"WhiteSmoke"}
            StateChangeScript {
                name:"stateScript13"
                script:{
                    literal.text="Cupressaceae - State 13"
                    picture.source="kiparisi/piramidalen.jpg"
                }
            }

        },
        State {
            name:"state14"
            PropertyChanges {target:top1;color:"Linen"}
            StateChangeScript {
                name:"stateScript14"
                script:{
                    literal.text="Cupressaceae - State 14"
                    picture.source="kiparisi/tuya.jpg"
                }
            }
        },
        State {
            name:"state15"
            PropertyChanges {target:top1;color:"MistyRose"}
            StateChangeScript {
                name:"stateScript15"
                script:{
                    literal.text="Cupressaceae - State 15"
                    picture.source="kiparisi/septe.jpg"
                }
            }
        },
        State {
            name:"state2"
            PropertyChanges {target:top1;color:"Cornsilk"}
            StateChangeScript {
                name:"stateScript2"
                script:{
                    literal.text="Lakes - State2"
                    picture.source="ezera/osam.jpg"
                }
            }
        },
        //States 3
        State {
            name:"state31"
            PropertyChanges {target:top1;color:"Cyan"}
            StateChangeScript {
                name:"stateScript31"
                script:{
                    literal.text="Rivers - State31"
                    picture.source="rivers/sea1.jpg"
                }
            }
        },
        State {
            name:"state32"
            PropertyChanges {target:top1;color:"LightCyan"}
            StateChangeScript {
                name:"stateScript32"
                script:{
                    literal.text="Rivers - State32"
                    picture.source="rivers/sea2.jpg"
                }
            }
        },
        State {
            name:"state33"
            PropertyChanges {target:top1;color:"LightBlue"}
            StateChangeScript {
                name:"stateScript33"
                script:{
                    literal.text="Rivers - State33"
                    picture.source="rivers/sea3.jpg"
                }
            }
        },
        State {
            name:"state34"
            PropertyChanges {target:top1;color:"LightSteelBlue"}
            StateChangeScript {
                name:"stateScript34"
                script:{
                    literal.text="Rivers - State34"
                    picture.source="rivers/vodopad.jpg"
                }
            }
        },
        State {
            name:"state35"
            PropertyChanges {target:top1;color:"DeepSkyBlue"}
            StateChangeScript {
                name:"stateScript35"
                script:{
                    literal.text="Rivers - State35"
                    picture.source="rivers/river.jpg"
                }
            }
        },
        State {
            name:"state4"
            PropertyChanges {target:top1;color:"LightGrey"}
            StateChangeScript {
                name:"stateScript4"
                script:{
                    literal.text="Lakes - State4"
                    picture.source="ezera/vacha.jpg"
                }
            }
        },
        State {
            name:"state51"
            PropertyChanges {target:top1;color:"LightYellow"}
            StateChangeScript {
                name:"stateScript51"
                script:{
                    literal.text="Roses - State51"
                    picture.source="roses/buquet.jpg"
                }
            }
        },
        State {
            name:"state52"
            PropertyChanges {target:top1;color:"Gold"}
            StateChangeScript {
                name:"stateScript52"
                script:{
                    literal.text="Roses - State52"
                    picture.source="roses/bush.jpg"
                }
            }

        },
        State {
            name:"state53"
            PropertyChanges {target:top1;color:"Khaki"}
            StateChangeScript {
                name:"stateScript53"
                script:{
                    literal.text="Roses - State53"
                    picture.source="roses/net.jpg"
                }
            }
        },
        State {
            name:"state54"
            PropertyChanges {target:top1;color:"Moccasin"}
            StateChangeScript {
                name:"stateScript54"
                script:{
                    literal.text="Roses - State54"
                    picture.source="roses/kalbo.jpg"
                }
            }
        },
        State {
            name:"state55"
            PropertyChanges {target:top1;color:"PeachPuff"}
            StateChangeScript {
                name:"stateScript55"
                script:{
                    literal.text="Roses - State55"
                    picture.source="roses/katerach.jpg"
                }
            }
        }
    ]
}
